#include "PipelineService.h"

#include "ShaderUtil.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/FrameBufferInstance.h"
#include "../runtime/PipelineInstance.h"
#include "../runtime/assets/MeshData.h"
#include "../runtime/assets/VertexData.h"
#include "../runtime/DescriptorInstance.h"

namespace Metal {
    void PipelineService::createPipelineLayout(const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                               const uint32_t pushConstantsSize, PipelineInstance *pipeline) const {
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        std::vector<VkDescriptorSetLayout> descriptorLayouts;
        descriptorLayouts.resize(descriptorSetsToBind.size());
        for (int i = 0; i < descriptorSetsToBind.size(); i++) {
            descriptorLayouts[i] = descriptorSetsToBind[i]->vkDescriptorSetLayout;
        }
        layoutInfo.pSetLayouts = descriptorLayouts.data();
        layoutInfo.setLayoutCount = descriptorLayouts.size();


        if (pushConstantsSize > 0) {
            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            pushConstantRange.offset = 0;
            pushConstantRange.size = pushConstantsSize;
            layoutInfo.pushConstantRangeCount = 1;
            layoutInfo.pPushConstantRanges = &pushConstantRange;
        }

        VulkanUtils::CheckVKResult(vkCreatePipelineLayout(vulkanContext.device.device, &layoutInfo, nullptr,
                                                          &pipeline->vkPipelineLayout));
    }

    void PipelineService::getBlendConfig(bool blendEnabled, VkPipelineColorBlendStateCreateInfo &colorBlending) {
        auto *colorBlendAttachment = new VkPipelineColorBlendAttachmentState;
        if (blendEnabled) {
            colorBlendAttachment->blendEnable = VK_TRUE; // Enable blending
            colorBlendAttachment->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Source color factor
            colorBlendAttachment->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Destination color factor
            colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD; // Blend operation (add source and destination)
            colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Source alpha factor
            colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Destination alpha factor
            colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD; // Blend operation for alpha
            colorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                   VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        } else {
            colorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                   VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment->blendEnable = VK_FALSE;
        }


        colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
    }

    PipelineInstance *PipelineService::createRenderingPipeline(FrameBufferInstance *frameBuffer,
                                                               VkCullModeFlagBits cullMode,
                                                               const char *vertexShader,
                                                               const char *fragmentShader,
                                                               const std::vector<DescriptorInstance *> &
                                                               descriptorSetsToBind,
                                                               const uint32_t pushConstantsSize,
                                                               bool blendEnabled,
                                                               bool prepareForMesh) {
        VkVertexInputBindingDescription *meshBindingDescription = nullptr;
        auto meshDescriptions = VertexData::GetAttributeDescriptions();

        auto *pipeline = new PipelineInstance();
        pipeline->pushConstantsSize = pushConstantsSize;
        auto fragmentShaderModule = ShaderUtil::CreateShaderModule(context, fragmentShader);
        auto vertexShaderModule = ShaderUtil::CreateShaderModule(context, vertexShader);
        registerResource(pipeline);
        createPipelineLayout(descriptorSetsToBind, pushConstantsSize, pipeline);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertexShaderModule;
        shaderStages[0].pName = "main";

        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragmentShaderModule;
        shaderStages[1].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        if (prepareForMesh) {
            meshBindingDescription = new VkVertexInputBindingDescription;
            meshBindingDescription->binding = 0;
            meshBindingDescription->stride = sizeof(VertexData);
            meshBindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(meshDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = meshDescriptions.data();
            vertexInputInfo.pVertexBindingDescriptions = meshBindingDescription;
        }

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = cullMode;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlending;
        getBlendConfig(blendEnabled, colorBlending);

        std::vector dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
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
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipeline->vkPipelineLayout;
        pipelineInfo.renderPass = frameBuffer->vkRenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        VulkanUtils::CheckVKResult(vkCreateGraphicsPipelines(vulkanContext.device.device, VK_NULL_HANDLE, 1,
                                                             &pipelineInfo,
                                                             nullptr,
                                                             &pipeline->vkPipeline));
        pipeline->descriptorSets = descriptorSetsToBind;
        delete meshBindingDescription;
        vkDestroyShaderModule(vulkanContext.device.device, fragmentShaderModule, nullptr);
        vkDestroyShaderModule(vulkanContext.device.device, vertexShaderModule, nullptr);
        delete colorBlending.pAttachments;
        return pipeline;
    }
} // Metal
