#include "CommandBufferRecorder.h"

#include <iostream>

#include "DescriptorInstance.h"
#include "FrameBufferAttachment.h"
#include "FrameBufferInstance.h"
#include "PipelineInstance.h"
#include "../ApplicationContext.h"
#include "../../engine/render-pass/AbstractRenderPass.h"
#include "../../common/util/VulkanUtils.h"

namespace Metal {
    void CommandBufferRecorder::createRenderPassInfo(FrameBufferInstance *frameBuffer, const bool clearBuffer) {
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = frameBuffer->vkRenderPass;
        renderPassInfo.framebuffer = frameBuffer->vkFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {
            frameBuffer->bufferWidth, frameBuffer->bufferHeight
        };
        if (clearBuffer) {
            for (const auto & attachment : frameBuffer->attachments) {
                VkClearValue &clearValue = clearColors.emplace_back();
                if (attachment->depth) {
                    clearValue.depthStencil.depth = 1.0f;
                    clearValue.depthStencil.stencil = 0;
                } else {
                    clearValue.color = {0.0f, 0.0f, 0.0f, 0.0f};
                }
            }
        }
        renderPassInfo.clearValueCount = clearColors.size();
        renderPassInfo.pClearValues = clearColors.data();
    }

    CommandBufferRecorder::CommandBufferRecorder(FrameBufferInstance *frameBuffer,
                                                 ApplicationContext &applicationContext,
                                                 const bool clearBuffer): context(applicationContext) {
        createRenderPassInfo(frameBuffer, clearBuffer);

        viewport.width = static_cast<float>(frameBuffer->bufferWidth);
        viewport.height = static_cast<float>(frameBuffer->bufferHeight);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = frameBuffer->bufferWidth;
        scissor.extent.height = frameBuffer->bufferHeight;

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = context.vulkanContext.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
        VulkanUtils::CheckVKResult(
            vkAllocateCommandBuffers(context.vulkanContext.device.device, &allocInfo, _commandBuffers.data()));
    }

    void CommandBufferRecorder::recordCommands(
        const std::vector<std::unique_ptr<AbstractRenderPass> > &renderPasses) const {
        auto vkCommandBuffer = _commandBuffers[context.getFrameIndex()];
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
        for (auto &pass: renderPasses) {
            if (!pass->shouldRun()) {
                continue;
            }
            pass->setCommandBuffer(vkCommandBuffer);
            if (pass->getPipeline() != nullptr) {
                vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pass->getPipeline()->vkPipeline);
                if (!pass->getPipeline()->descriptorSets.empty()) {
                    vkCmdBindDescriptorSets(vkCommandBuffer,
                                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            pass->getPipeline()->vkPipelineLayout,
                                            0,
                                            pass->getPipeline()->descriptorSets.size(),
                                            pass->getPipeline()->descriptorSets.data(),
                                            0,
                                            nullptr);
                }
            }
            pass->onSync();
        }
        vkCmdEndRenderPass(vkCommandBuffer);
        vkEndCommandBuffer(vkCommandBuffer);

        context.vulkanContext.pushCommandBuffer(vkCommandBuffer);
    }
} // Metal
