#include "CommandBufferInstance.h"

#include "../pipeline/PipelineInstance.h"
#include "../framebuffer/FrameBufferInstance.h"

namespace Metal {
    VkRenderPassBeginInfo CommandBufferInstance::startMapping(const PipelineInstance *pipeline) const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(buffer, &beginInfo);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline->renderPass->vkRenderPass;
        renderPassInfo.framebuffer = pipeline->renderPass->vkFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {pipeline->renderPass->bufferWidth, pipeline->renderPass->bufferHeight};

        constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        return renderPassInfo;
    }

    void CommandBufferInstance::stopMapping() const {
        // EXAMPLE
        // vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);
        // vkCmdDraw(buffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(buffer);
        vkEndCommandBuffer(buffer);
    }
}
