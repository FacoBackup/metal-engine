#include "PipelineService.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/FrameBufferInstance.h"
#include "../runtime/PipelineInstance.h"
#include "../runtime/DescriptorInstance.h"

namespace Metal {
    void PipelineService::createPipelineLayout(const std::vector<DescriptorInstance *> &descriptorSetsToBind, const uint32_t pushConstantsSize, PipelineInstance *pipeline) {
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

    PipelineInstance *PipelineService::createRenderingPipeline(FrameBufferInstance *frameBuffer,
                                                               VkCullModeFlagBits cullMode,
                                                               const char *vertexShader,
                                                               const char *fragmentShader,
                                                               const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                                               const uint32_t pushConstantsSize) {
        auto *pipeline = new PipelineInstance();
        pipeline->pushConstantsSize = pushConstantsSize;
        pipeline->frameBuffer = frameBuffer;
        pipeline->fragmentShader = context.getVulkanContext().shaderService.createShaderModule(fragmentShader);
        pipeline->vertexShader = context.getVulkanContext().shaderService.createShaderModule(vertexShader);
        registerResource(pipeline);
        createPipelineLayout(descriptorSetsToBind, pushConstantsSize, pipeline);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = pipeline->vertexShader->vkShaderModule;
        shaderStages[0].pName = "main";

        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = pipeline->fragmentShader->vkShaderModule;
        shaderStages[1].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

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

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
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
        createCommandBuffer(pipeline);
        return pipeline;
    }

    /**
 * Commands still need to be recorded
 */
    void *PipelineService::createCommandBuffer(PipelineInstance *pipeline) const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = context.getVulkanContext().commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer{};
        VulkanUtils::CheckVKResult(
            vkAllocateCommandBuffers(vulkanContext.device.device, &allocInfo, &commandBuffer));

        pipeline->vkCommandBuffer = commandBuffer;
    }
} // Metal
