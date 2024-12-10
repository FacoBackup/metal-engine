#include "CommandBufferInstance.h"

#include "PipelineInstance.h"
#include "FrameBufferInstance.h"

namespace Metal {
    void CommandBufferInstance::startRecording() {
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
                                    0, // firstSet
                                    1, // descriptorSetCount
                                    &pipeline->descriptorSet,
                                    0, // dynamicOffsetCount
                                    nullptr); // pDynamicOffsets
        }

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void CommandBufferInstance::stopRecording() const {
        vkCmdEndRenderPass(vkCommandBuffer);
        vkEndCommandBuffer(vkCommandBuffer);
    }

    void CommandBufferInstance::recordPushConstant(void *data) const {
        vkCmdPushConstants(
            vkCommandBuffer,
            pipeline->vkPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            pipeline->pushConstantsSize,
            data);
    }

    void CommandBufferInstance::recordDrawSimpleInstanced(const uint32_t vertexCount, const uint32_t instanceCount) const {
        vkCmdDraw(vkCommandBuffer, vertexCount, instanceCount, 0, 0);
    }
}
