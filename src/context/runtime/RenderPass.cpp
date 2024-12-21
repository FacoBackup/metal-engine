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
        constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 0}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;


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

            std::vector<VkDescriptorSet> sets;
            sets.resize(pass->getPipeline()->descriptorSets.size());
            for (int i = 0; i < pass->getPipeline()->descriptorSets.size(); i++) {
                sets[i] = pass->getPipeline()->descriptorSets[i]->vkDescriptorSet;
            }
            if (!pass->getPipeline()->descriptorSets.empty()) {
                vkCmdBindDescriptorSets(vkCommandBuffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pass->getPipeline()->vkPipelineLayout,
                                        0,
                                        sets.size(),
                                        sets.data(),
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
