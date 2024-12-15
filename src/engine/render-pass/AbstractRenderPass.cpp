#include "AbstractRenderPass.h"
#include "../../context/ApplicationContext.h"
#include "../../context/runtime/PipelineInstance.h"
#include "../../context/runtime/MeshInstance.h"
#include "../../context/runtime/BufferInstance.h"

namespace Metal {
    void AbstractRenderPass::recordPushConstant(const void *data) {
        vkCmdPushConstants(
            vkCommandBuffer,
            getPipeline()->vkPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            getPipeline()->pushConstantsSize,
            data);
    }

    void AbstractRenderPass::recordDrawSimpleInstanced(const uint32_t vertexCount, const uint32_t instanceCount) const {
        vkCmdDraw(vkCommandBuffer, vertexCount, instanceCount, 0, 0);
    }

    void AbstractRenderPass::bindMesh(const MeshInstance *instance, const uint32_t instanceCount) const {
        constexpr VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, &instance->dataBuffer->vkBuffer, offsets);
        vkCmdBindIndexBuffer(vkCommandBuffer, instance->indexBuffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(vkCommandBuffer, instance->indexCount, instanceCount, 0, 0, 0);
    }
}
