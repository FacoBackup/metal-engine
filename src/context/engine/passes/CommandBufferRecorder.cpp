#include "CommandBufferRecorder.h"

#include <iostream>

#include "../../../service/descriptor/DescriptorInstance.h"
#include "../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../service/pipeline/PipelineInstance.h"
#include "../../ApplicationContext.h"
#include "../render-pass/AbstractRenderPass.h"
#include "../../../util/VulkanUtils.h"

namespace Metal {

    void CommandBufferRecorder::createCommandBuffer() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = context.vulkanContext.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
        VulkanUtils::CheckVKResult(
            vkAllocateCommandBuffers(context.vulkanContext.device.device, &allocInfo, _commandBuffers.data()));
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

        createCommandBuffer();
        computePassMode = false;
    }

    CommandBufferRecorder::CommandBufferRecorder(ApplicationContext &applicationContext) : context(applicationContext) {
        createCommandBuffer();
        computePassMode = true;
    }

    void CommandBufferRecorder::createRenderPassInfo(const FrameBufferInstance *frameBuffer, const bool clearBuffer) {
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = frameBuffer->vkRenderPass;
        renderPassInfo.framebuffer = frameBuffer->vkFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {
            frameBuffer->bufferWidth, frameBuffer->bufferHeight
        };
        if (clearBuffer) {
            for (const auto &attachment: frameBuffer->attachments) {
                VkClearValue &clearValue = clearColors.emplace_back();
                if (attachment->depth) {
                    clearValue.depthStencil.depth = 1.0f;
                    clearValue.depthStencil.stencil = 0;
                } else {
                    clearValue.color = {
                        frameBuffer->clearColor.r, frameBuffer->clearColor.g, frameBuffer->clearColor.b,
                        frameBuffer->clearColor.a
                    };
                }
            }
        }
        renderPassInfo.clearValueCount = clearColors.size();
        renderPassInfo.pClearValues = clearColors.data();
    }

    void CommandBufferRecorder::recordCommands(
        const std::vector<AbstractPass *> &passes) const {
        auto vkCommandBuffer = _commandBuffers[context.getFrameIndex()];
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        if (computePassMode) {
            RecordCommandsInternal(passes, vkCommandBuffer);
        } else {
            vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
            RecordCommandsInternal(passes, vkCommandBuffer);
            vkCmdEndRenderPass(vkCommandBuffer);
        }

        vkEndCommandBuffer(vkCommandBuffer);
        context.vulkanContext.pushCommandBuffer(vkCommandBuffer);
    }

    void CommandBufferRecorder::RecordCommandsInternal(
        const std::vector<AbstractPass *> &passes,
        VkCommandBuffer vkCommandBuffer) {
        for (auto &pass: passes) {
            pass->setCommandBuffer(vkCommandBuffer);
            if (!pass->shouldRun()) {
                continue;
            }
            if (pass->getPipeline() != nullptr) {
                vkCmdBindPipeline(vkCommandBuffer, pass->getBindingPoint(), pass->getPipeline()->vkPipeline);
            }
            pass->bindStaticDescriptorSets();
            pass->onSync();
        }
    }
} // Metal
