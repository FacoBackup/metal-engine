#include "CommandBufferInstance.h"

#include "../pipeline/PipelineInstance.h"
#include "../framebuffer/FrameBufferInstance.h"

namespace Metal {
    void CommandBufferInstance::startMapping() {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline->frameBuffer->vkRenderPass;
        renderPassInfo.framebuffer = pipeline->frameBuffer->vkFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {pipeline->frameBuffer->bufferWidth, pipeline->frameBuffer->bufferHeight};

        constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);
        if (pipeline->descriptorSet != VK_NULL_HANDLE) {
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipelineLayout,
                                    0,  // firstSet
                                    1,  // descriptorSetCount
                                    &pipeline->descriptorSet,
                                    0,	// dynamicOffsetCount
                                    nullptr);	// pDynamicOffsets
        }

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void CommandBufferInstance::stopMapping() const {
        vkCmdEndRenderPass(vkCommandBuffer);
        vkEndCommandBuffer(vkCommandBuffer);
    }
}
