#include "PipelineInstance.h"

#include "BufferInstance.h"
#include "DescriptorInstance.h"
#include "MeshInstance.h"
#include "FrameBufferInstance.h"

namespace Metal {
    void PipelineInstance::startRecording() {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = frameBuffer->vkRenderPass;
        renderPassInfo.framebuffer = frameBuffer->vkFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {frameBuffer->bufferWidth, frameBuffer->bufferHeight};

        constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

        std::vector<VkDescriptorSet> sets;
        sets.resize(descriptorSets.size());
        for (int i = 0; i < descriptorSets.size(); i++) {
            sets[i] = descriptorSets[i]->vkDescriptorSet;
        }
        vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout,
                                0,
                                sets.size(),
                                sets.data(),
                                0,
                                nullptr);

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void PipelineInstance::stopRecording() const {
        vkCmdEndRenderPass(vkCommandBuffer);
        vkEndCommandBuffer(vkCommandBuffer);
    }

    void PipelineInstance::recordPushConstant(void *data) const {
        vkCmdPushConstants(
            vkCommandBuffer,
            vkPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            pushConstantsSize,
            data);
    }

    void PipelineInstance::recordDrawSimpleInstanced(const uint32_t vertexCount, const uint32_t instanceCount) const {
        vkCmdDraw(vkCommandBuffer, vertexCount, instanceCount, 0, 0);
    }

    void PipelineInstance::dispose(VulkanContext &context) {
        vkDestroyPipelineLayout(context.device.device, vkPipelineLayout, nullptr);
        vkDestroyPipeline(context.device.device, vkPipeline, nullptr);
    }

    void PipelineInstance::bindMesh(const MeshInstance *instance, const uint32_t instanceCount) const {
        constexpr VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, &instance->dataBuffer->vkBuffer, offsets);
        vkCmdBindIndexBuffer(vkCommandBuffer, instance->indexBuffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(vkCommandBuffer, instance->indexCount, instanceCount, 0, 0, 0);
    }
}
