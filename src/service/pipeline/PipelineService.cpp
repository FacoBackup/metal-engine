#include "PipelineService.h"

#include "PipelineBuilder.h"
#include "../../util/ShaderUtil.h"
#include "../../context/ApplicationContext.h"
#include "../../util/VulkanUtils.h"
#include "../framebuffer/FrameBufferInstance.h"
#include "../pipeline/PipelineInstance.h"
#include "../mesh/MeshData.h"
#include "../mesh/VertexData.h"
#include "../descriptor/DescriptorInstance.h"
#include "../framebuffer/FrameBufferAttachment.h"
#include "../buffer/BufferInstance.h"

namespace Metal {
    void PipelineService::createPipelineLayout(
        const unsigned int pushConstantsSize, PipelineInstance *pipeline) const {
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        std::array descriptorLayouts{pipeline->descriptor.get()->vkDescriptorSetLayout};
        layoutInfo.pSetLayouts = descriptorLayouts.data();
        layoutInfo.setLayoutCount = 1;

        if (pushConstantsSize > 0) {
            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags = pipeline->isRayTracing
                                               ? (VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR |
                                                  VK_SHADER_STAGE_MISS_BIT_KHR)
                                               : pipeline->isCompute
                                                     ? VK_SHADER_STAGE_COMPUTE_BIT
                                                     : VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            pushConstantRange.offset = 0;
            pushConstantRange.size = pushConstantsSize;
            layoutInfo.pushConstantRangeCount = 1;
            layoutInfo.pPushConstantRanges = &pushConstantRange;
        }

        VulkanUtils::CheckVKResult(vkCreatePipelineLayout(CTX.vulkanContext.device.device, &layoutInfo, nullptr,
                                                          &pipeline->vkPipelineLayout));
    }

    PipelineInstance *PipelineService::createPipeline(PipelineBuilder &pipelineBuilder) {
        auto id = pipelineBuilder.id != nullptr ? std::string(pipelineBuilder.id) : Util::uuidV4();
        auto *pipeline = createResourceInstance(id);

        if (!pipelineBuilder.resourceBindings.empty()) {
            VkShaderStageFlags stageFlags = 0;
            if (pipelineBuilder.isRayTracing) {
                stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR |
                             VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR |
                             VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
            } else if (pipelineBuilder.computeShader != nullptr) {
                stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            } else {
                stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            }

            if (!pipelineBuilder.resourceBindings.empty()) {
                auto descriptorInstance = std::make_unique<DescriptorInstance>();
                for (auto &binding: pipelineBuilder.resourceBindings) {
                    if (binding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR || binding.
                        accelerationStructure != VK_NULL_HANDLE) {
                        binding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                    } else if (binding.bufferInstance != nullptr) {
                        binding.descriptorType = binding.bufferInstance->getBufferType() == BufferType::UNIFORM_BUFFER
                                                     ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                                                     : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    } else if (binding.view != VK_NULL_HANDLE) {
                        binding.descriptorType = binding.sampler != VK_NULL_HANDLE
                                                     ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
                                                     : VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    }
                    binding.stageFlags = static_cast<VkShaderStageFlagBits>(stageFlags);
                    descriptorInstance->addLayoutBinding(binding);
                }
                descriptorInstance->create();
                pipeline->descriptor = std::move(descriptorInstance);
            }
        }

        if (pipelineBuilder.isRayTracing) {
            return createRayTracingPipeline(pipelineBuilder, pipeline);
        }
        if (pipelineBuilder.computeShader != nullptr) {
            return createComputePipeline(pipelineBuilder, pipeline);
        }
        return createRenderingPipeline(pipelineBuilder, pipeline);
    }

    PipelineInstance *PipelineService::createComputePipeline(const PipelineBuilder &pipelineBuilder,
                                                             PipelineInstance *pipeline) {
        pipeline->isCompute = true;
        pipeline->pushConstantsSize = pipelineBuilder.pushConstantsSize;
        VkShaderModule computeShaderModule = ShaderUtil::CreateShaderModule(pipelineBuilder.computeShader);
        VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
        computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageInfo.module = computeShaderModule;
        computeShaderStageInfo.pName = "main";

        createPipelineLayout(pipelineBuilder.pushConstantsSize, pipeline);

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = pipeline->vkPipelineLayout;
        pipelineInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(CTX.vulkanContext.device.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                     &pipeline->vkPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        vkDestroyShaderModule(CTX.vulkanContext.device.device, computeShaderModule, nullptr);
        return pipeline;
    }

    PipelineInstance *PipelineService::createRenderingPipeline(PipelineBuilder &pipelineBuilder,
                                                               PipelineInstance *pipeline) {
        auto meshDescriptions = VertexData::GetAttributeDescriptions();

        pipeline->pushConstantsSize = pipelineBuilder.pushConstantsSize;
        auto fragmentShaderModule = ShaderUtil::CreateShaderModule(pipelineBuilder.fragmentShader);
        auto vertexShaderModule = ShaderUtil::CreateShaderModule(pipelineBuilder.vertexShader);
        createPipelineLayout(pipelineBuilder.pushConstantsSize, pipeline);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertexShaderModule;
        shaderStages[0].pName = "main";
        // shaderStages[0].pSpecializationInfo;

        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragmentShaderModule;
        shaderStages[1].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkVertexInputBindingDescription meshBindingDescription{};
        if (pipelineBuilder.prepareForMesh) {
            meshBindingDescription.binding = 0;
            meshBindingDescription.stride = sizeof(VertexData);
            meshBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<unsigned int>(meshDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = meshDescriptions.data();
            vertexInputInfo.pVertexBindingDescriptions = &meshBindingDescription;
        }

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = pipelineBuilder.useStrip
                                     ? VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
                                     : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = pipelineBuilder.cullMode;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
        for (int i = 0; i < pipelineBuilder.frameBuffer->attachments.size(); i++) {
            auto &colorBlendAttachment = colorBlendAttachments.emplace_back();
            if (pipelineBuilder.blendEnabled && !pipelineBuilder.frameBuffer->attachments[i]->depth) {
                colorBlendAttachment.blendEnable = VK_TRUE;
                colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            } else {
                colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable = VK_FALSE;
            }
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = colorBlendAttachments.size();
        colorBlending.pAttachments = colorBlendAttachments.data();
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = dynamicStates.size();
        dynamicState.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;

        VkPipelineDepthStencilStateCreateInfo depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };
        pipelineInfo.pDepthStencilState = pipelineBuilder.depthTest ? &depthStencilState : nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipeline->vkPipelineLayout;
        pipelineInfo.renderPass = pipelineBuilder.frameBuffer->vkRenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        VulkanUtils::CheckVKResult(vkCreateGraphicsPipelines(CTX.vulkanContext.device.device, VK_NULL_HANDLE, 1,
                                                             &pipelineInfo,
                                                             nullptr,
                                                             &pipeline->vkPipeline));

        vkDestroyShaderModule(CTX.vulkanContext.device.device, fragmentShaderModule, nullptr);
        vkDestroyShaderModule(CTX.vulkanContext.device.device, vertexShaderModule, nullptr);
        return pipeline;
    }

    static VkDeviceAddress
    getBufferDeviceAddress(VkDevice device, VkBuffer buffer, PFN_vkGetBufferDeviceAddressKHR fn) {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return fn(device, &info);
    }

    PipelineInstance *PipelineService::createRayTracingPipeline(const PipelineBuilder &pipelineBuilder,
                                                                PipelineInstance *pipeline) {
        pipeline->isRayTracing = true;
        pipeline->pushConstantsSize = pipelineBuilder.pushConstantsSize;

        // Create shader modules
        auto rayGenModule = ShaderUtil::CreateShaderModule(pipelineBuilder.rayGenShader);
        auto missModule = ShaderUtil::CreateShaderModule(pipelineBuilder.missShader);
        auto closestHitModule = ShaderUtil::CreateShaderModule(pipelineBuilder.closestHitShader);

        // Shader stages: 0=raygen, 1=miss, 2=closesthit
        std::array<VkPipelineShaderStageCreateInfo, 3> shaderStages{};
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        shaderStages[0].module = rayGenModule;
        shaderStages[0].pName = "main";

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_MISS_BIT_KHR;
        shaderStages[1].module = missModule;
        shaderStages[1].pName = "main";

        shaderStages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[2].stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        shaderStages[2].module = closestHitModule;
        shaderStages[2].pName = "main";

        // Shader groups
        std::array<VkRayTracingShaderGroupCreateInfoKHR, 3> shaderGroups{};
        // Raygen group
        shaderGroups[0].sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        shaderGroups[0].type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        shaderGroups[0].generalShader = 0;
        shaderGroups[0].closestHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroups[0].anyHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroups[0].intersectionShader = VK_SHADER_UNUSED_KHR;

        // Miss group
        shaderGroups[1].sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        shaderGroups[1].type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        shaderGroups[1].generalShader = 1;
        shaderGroups[1].closestHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroups[1].anyHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroups[1].intersectionShader = VK_SHADER_UNUSED_KHR;

        // Closest hit group
        shaderGroups[2].sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        shaderGroups[2].type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        shaderGroups[2].generalShader = VK_SHADER_UNUSED_KHR;
        shaderGroups[2].closestHitShader = 2;
        shaderGroups[2].anyHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroups[2].intersectionShader = VK_SHADER_UNUSED_KHR;

        createPipelineLayout(pipelineBuilder.pushConstantsSize, pipeline);

        VkRayTracingPipelineCreateInfoKHR rtPipelineInfo{};
        rtPipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
        rtPipelineInfo.stageCount = shaderStages.size();
        rtPipelineInfo.pStages = shaderStages.data();
        rtPipelineInfo.groupCount = shaderGroups.size();
        rtPipelineInfo.pGroups = shaderGroups.data();
        rtPipelineInfo.maxPipelineRayRecursionDepth = 1;
        rtPipelineInfo.layout = pipeline->vkPipelineLayout;

        VulkanUtils::CheckVKResult(
            CTX.vulkanContext.vkCreateRayTracingPipelinesKHR(
                CTX.vulkanContext.device.device, VK_NULL_HANDLE, VK_NULL_HANDLE,
                1, &rtPipelineInfo, nullptr, &pipeline->vkPipeline));

        // Build Shader Binding Table
        const auto &rtProps = CTX.vulkanContext.rayTracingPipelineProperties;
        const uint32_t handleSize = rtProps.shaderGroupHandleSize;
        const uint32_t handleAlignment = rtProps.shaderGroupHandleAlignment;
        const uint32_t baseAlignment = rtProps.shaderGroupBaseAlignment;
        const uint32_t handleSizeAligned = (handleSize + handleAlignment - 1) & ~(handleAlignment - 1);

        const uint32_t groupCount = 3;
        const uint32_t sbtSize = groupCount * handleSizeAligned;
        std::vector<uint8_t> shaderHandleStorage(sbtSize);
        VulkanUtils::CheckVKResult(
            CTX.vulkanContext.vkGetRayTracingShaderGroupHandlesKHR(
                CTX.vulkanContext.device.device, pipeline->vkPipeline,
                0, groupCount, sbtSize, shaderHandleStorage.data()));

        // Create SBT buffers - each needs baseAlignment
        auto createSBTBuffer = [&](uint32_t groupIndex, const std::string &sbtType) -> BufferInstance * {
            const uint32_t sbtBufferSize = (handleSizeAligned + baseAlignment - 1) & ~(baseAlignment - 1);
            auto *buf = CTX.bufferService.createBuffer(
                pipeline->getId() + "_sbt_" + sbtType,
                sbtBufferSize,
                VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                true);
            memcpy(buf->mapped, shaderHandleStorage.data() + groupIndex * handleSizeAligned, handleSize);
            return buf;
        };

        pipeline->raygenSBT = createSBTBuffer(0, "raygen");
        pipeline->missSBT = createSBTBuffer(1, "miss");
        pipeline->hitSBT = createSBTBuffer(2, "hit");

        auto getAddr = [&](const BufferInstance *buf) {
            return getBufferDeviceAddress(CTX.vulkanContext.device.device, buf->vkBuffer,
                                          CTX.vulkanContext.vkGetBufferDeviceAddressKHR);
        };

        pipeline->raygenRegion.deviceAddress = getAddr(pipeline->raygenSBT);
        pipeline->raygenRegion.stride = handleSizeAligned;
        pipeline->raygenRegion.size = handleSizeAligned;

        pipeline->missRegion.deviceAddress = getAddr(pipeline->missSBT);
        pipeline->missRegion.stride = handleSizeAligned;
        pipeline->missRegion.size = handleSizeAligned;

        pipeline->hitRegion.deviceAddress = getAddr(pipeline->hitSBT);
        pipeline->hitRegion.stride = handleSizeAligned;
        pipeline->hitRegion.size = handleSizeAligned;

        pipeline->callableRegion = {}; // unused

        vkDestroyShaderModule(CTX.vulkanContext.device.device, rayGenModule, nullptr);
        vkDestroyShaderModule(CTX.vulkanContext.device.device, missModule, nullptr);
        vkDestroyShaderModule(CTX.vulkanContext.device.device, closestHitModule, nullptr);

        return pipeline;
    }

    void PipelineService::disposeResource(PipelineInstance *resource) {
        LOG_INFO("Disposing of pipeline instance");

        vkDestroyPipelineLayout(CTX.vulkanContext.device.device, resource->vkPipelineLayout, nullptr);
        vkDestroyPipeline(CTX.vulkanContext.device.device, resource->vkPipeline, nullptr);
    }

    std::vector<DescriptorInstance *> PipelineService::getAllDescriptors() const {
        std::vector<DescriptorInstance *> descriptors;
        std::lock_guard lock(resourceMutex);
        for (auto &[id, pipeline]: resources) {
            if (pipeline->descriptor != nullptr) {
                descriptors.push_back(pipeline->descriptor.get());
            }
        }
        return descriptors;
    }
} // Metal
