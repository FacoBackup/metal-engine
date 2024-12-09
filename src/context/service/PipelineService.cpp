#include "PipelineService.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/FrameBufferInstance.h"
#include "../runtime/PipelineInstance.h"
#include "../runtime/DescriptorInstance.h"

namespace Metal {
    PipelineInstance *PipelineService::createRenderingPipeline(FrameBufferInstance *frameBuffer,
                                                               const char *vertexShader,
                                                               const char *fragmentShader,
                                                               const std::vector<DescriptorInstance *> &descriptors,
                                                               const uint32_t pushConstantsSize) const {
        auto *pipeline = new PipelineInstance();
        pipeline->pushConstantsSize = pushConstantsSize;
        pipeline->frameBuffer = frameBuffer;
        pipeline->fragmentShader = context.getVulkanContext().shaderService.createShaderModule(fragmentShader);
        pipeline->vertexShader = context.getVulkanContext().shaderService.createShaderModule(vertexShader);
        registerResource(pipeline);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = pipeline->vertexShader->vkShaderModule;
        shaderStages[0].pName = "main";

        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = pipeline->fragmentShader->vkShaderModule;
        shaderStages[1].pName = "main";

        VkPipelineVertexInputStateCreateInfo VertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
        };

        VkPipelineInputAssemblyStateCreateInfo PipelineIACreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        VkViewport VP = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(frameBuffer->bufferWidth),
            .height = static_cast<float>(frameBuffer->bufferHeight),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D Scissor{
            .offset = {
                .x = 0,
                .y = 0,
            },
            .extent = {
                .width = frameBuffer->bufferWidth,
                .height = frameBuffer->bufferHeight
            }
        };

        VkPipelineViewportStateCreateInfo VPCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &VP,
            .scissorCount = 1,
            .pScissors = &Scissor
        };

        VkPipelineRasterizationStateCreateInfo RastCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .lineWidth = 1.0f
        };

        VkPipelineMultisampleStateCreateInfo PipelineMSCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f
        };

        VkPipelineColorBlendAttachmentState BlendAttachState = {
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                              VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo BlendCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &BlendAttachState
        };


        // BIND LAYOUTS
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        std::vector<VkDescriptorSetLayout> descriptorLayouts;
        descriptorLayouts.resize(descriptors.size());
        for (int i = 0; i < descriptors.size(); i++) {
            descriptorLayouts[i] = descriptors[i]->vkDescriptorSetLayout;
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
        // BIND LAYOUTS


        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = shaderStages.size();
        pipelineCreateInfo.pStages = shaderStages.data();
        pipelineCreateInfo.pVertexInputState = &VertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &PipelineIACreateInfo;
        pipelineCreateInfo.pViewportState = &VPCreateInfo;
        pipelineCreateInfo.pRasterizationState = &RastCreateInfo;
        pipelineCreateInfo.pMultisampleState = &PipelineMSCreateInfo;
        pipelineCreateInfo.pColorBlendState = &BlendCreateInfo;
        pipelineCreateInfo.layout = pipeline->vkPipelineLayout;
        pipelineCreateInfo.renderPass = frameBuffer->vkRenderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;
        pipelineCreateInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;

        VulkanUtils::CheckVKResult(vkCreateGraphicsPipelines(vulkanContext.device.device, VK_NULL_HANDLE, 1,
                                                             &pipelineCreateInfo,
                                                             nullptr,
                                                             &pipeline->vkPipeline));

        pipeline->commandBuffer = context.getVulkanContext().commandService.createCommandBuffer(pipeline);
        return pipeline;
    }
} // Metal
