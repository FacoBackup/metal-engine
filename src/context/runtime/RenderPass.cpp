#include "RenderPass.h"

#include "DescriptorInstance.h"
#include "FrameBufferInstance.h"
#include "PipelineInstance.h"
#include "../ApplicationContext.h"
#include "../../engine/render-pass/AbstractRenderPass.h"
#include "../../common/util/VulkanUtils.h"

namespace Metal {
    RenderPass::RenderPass(const FrameBufferInstance *frameBuffer,
                           ApplicationContext &applicationContext): context(applicationContext) {
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = frameBuffer->vkRenderPass;
        renderPassInfo.framebuffer = frameBuffer->vkFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {
            frameBuffer->bufferWidth, frameBuffer->bufferHeight
        };

        std::vector<VkClearValue> clearColors{};
        for (int i = 0; i<frameBuffer->attachments.size(); i++) {
            clearColors.push_back({{0.0f, 0.0f, 0.0f, 0}});
        }
        renderPassInfo.clearValueCount = clearColors.size();
        renderPassInfo.pClearValues = clearColors.data();


        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = context.vulkanContext.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VulkanUtils::CheckVKResult(
            vkAllocateCommandBuffers(context.vulkanContext.device.device, &allocInfo, &vkCommandBuffer));
    }

    void RenderPass::recordCommands(const std::vector<std::unique_ptr<AbstractRenderPass> > &renderPasses) const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        for (auto &pass: renderPasses) {
            pass->setCommandBuffer(vkCommandBuffer);
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
            pass->onSync();
        }
        vkCmdEndRenderPass(vkCommandBuffer);
        vkEndCommandBuffer(vkCommandBuffer);

        context.vulkanContext.getFrameData().commandBuffers.push_back(vkCommandBuffer);
    }
} // Metal
