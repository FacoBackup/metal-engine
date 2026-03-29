#include "CommandBufferRecorder.h"
#include <mutex>


#include "../resource/RenderTargetAttachment.h"
#include "../resource/RenderTargetInstance.h"
#include "../resource/PipelineInstance.h"
#include "../../ApplicationContext.h"
#include "AbstractRenderPass.h"
#include "../../common/VulkanUtils.h"
#include "../../core/VulkanContext.h"
#include "../../core/FrameService.h"

namespace Metal {
    void CommandBufferRecorder::onInitialize() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vulkanContext->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
        VulkanUtils::CheckVKResult(
            vkAllocateCommandBuffers(vulkanContext->device.device, &allocInfo,
                                     _commandBuffers.data()));
    }

    CommandBufferRecorder::CommandBufferRecorder(std::string id, RenderTargetInstance *frameBuffer,
                                                 const bool clearBuffer) : RuntimeResource(std::move(id)) {
        attachments = frameBuffer->attachments;
        createRenderingInfo(frameBuffer, clearBuffer);

        viewport.width = static_cast<float>(frameBuffer->bufferWidth);
        viewport.height = static_cast<float>(frameBuffer->bufferHeight);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = frameBuffer->bufferWidth;
        scissor.extent.height = frameBuffer->bufferHeight;

        computePassMode = false;
    }

    CommandBufferRecorder::CommandBufferRecorder(const std::string &id) : RuntimeResource(id) {
        computePassMode = true;
    }

    void CommandBufferRecorder::createRenderingInfo(const RenderTargetInstance *frameBuffer, const bool clearBuffer) {
        colorAttachments.clear();
        hasDepth = false;

        for (const auto &attachment: frameBuffer->attachments) {
            VkRenderingAttachmentInfo attachmentInfo{};
            attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachmentInfo.imageView = attachment->vkImageView;
            attachmentInfo.imageLayout = attachment->depth
                                             ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                             : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachmentInfo.loadOp = clearBuffer ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            if (attachment->depth) {
                attachmentInfo.clearValue.depthStencil = {1.0f, 0};
                depthAttachment = attachmentInfo;
                hasDepth = true;
            } else {
                attachmentInfo.clearValue.color = {
                    frameBuffer->clearColor.r, frameBuffer->clearColor.g, frameBuffer->clearColor.b,
                    frameBuffer->clearColor.a
                };
                colorAttachments.push_back(attachmentInfo);
            }
        }

        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = {frameBuffer->bufferWidth, frameBuffer->bufferHeight};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = colorAttachments.size();
        renderingInfo.pColorAttachments = colorAttachments.data();
        renderingInfo.pDepthAttachment = hasDepth ? &depthAttachment : nullptr;
        renderingInfo.pStencilAttachment = nullptr;
    }

    void CommandBufferRecorder::recordCommands(
        const std::vector<std::unique_ptr<AbstractPass>> &passes,
        const RGResourceManager& resources) {
        auto vkCommandBuffer = _commandBuffers[frameService->getFrameIndex()];
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        if (computePassMode) {
            RecordCommandsInternal(passes, vkCommandBuffer, resources);
        } else {
            for (const auto &attachment: attachments) {
                VkImageMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                barrier.newLayout = attachment->depth
                                        ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                        : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = attachment->vkImage;
                barrier.subresourceRange.aspectMask = attachment->depth
                                                          ? VK_IMAGE_ASPECT_DEPTH_BIT
                                                          : VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = attachment->depth
                                            ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                                            : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

                vkCmdPipelineBarrier(
                    vkCommandBuffer,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                    attachment->depth
                        ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
                        : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
                );
            }
            renderingInfo.pColorAttachments = colorAttachments.data();
            renderingInfo.pDepthAttachment = hasDepth ? &depthAttachment : nullptr;

            vulkanContext->vkCmdBeginRendering(vkCommandBuffer, &renderingInfo);
            vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
            RecordCommandsInternal(passes, vkCommandBuffer, resources);
            vulkanContext->vkCmdEndRendering(vkCommandBuffer);

            for (const auto &attachment: attachments) {
                VkImageMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.oldLayout = attachment->depth
                                        ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                        : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = attachment->vkImage;
                barrier.subresourceRange.aspectMask = attachment->depth
                                                          ? VK_IMAGE_ASPECT_DEPTH_BIT
                                                          : VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                barrier.srcAccessMask = attachment->depth
                                            ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                                            : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(
                    vkCommandBuffer,
                    attachment->depth
                        ? VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
                        : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
                );
            }
        }

        vkEndCommandBuffer(vkCommandBuffer);
        vulkanContext->pushCommandBuffer(vkCommandBuffer);
    }

    void CommandBufferRecorder::RecordCommandsInternal(
        const std::vector<std::unique_ptr<AbstractPass>> &passes,
        VkCommandBuffer vkCommandBuffer,
        const RGResourceManager& resources) {
        for (auto &pass: passes) {
            pass->setCommandBuffer(vkCommandBuffer);
            if (!pass->shouldRun()) {
                continue;
            }
            
            pass->getBarrierBatch().execute(vkCommandBuffer);

            if (pass->getPipeline() != nullptr) {
                vkCmdBindPipeline(vkCommandBuffer, pass->getBindingPoint(), pass->getPipeline()->vkPipeline);
            }
            pass->bindStaticDescriptorSets();
            pass->onSync();
            pass->execute(vkCommandBuffer, resources);
        }
    }
} // Metal
