#include "CommandBufferInstance.h"

#include "../pipeline/PipelineInstance.h"
#include "../framebuffer/FrameBufferInstance.h"

namespace Metal {
    void CommandBufferInstance::startMapping() {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(vkBuffer, &beginInfo);

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline->frameBuffer->vkRenderPass;
        renderPassInfo.framebuffer = pipeline->frameBuffer->vkFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {pipeline->frameBuffer->bufferWidth, pipeline->frameBuffer->bufferHeight};

        constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(vkBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void CommandBufferInstance::stopMapping() const {
        // EXAMPLE
        // vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);
        // vkCmdDraw(buffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(vkBuffer);
        vkEndCommandBuffer(vkBuffer);
    }
}
