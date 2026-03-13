#include "AbstractRenderPass.h"
#include "../../ApplicationContext.h"
#include "../resource/MeshInstance.h"
#include "../resource/BufferInstance.h"

namespace Metal {
    AbstractRenderPass::AbstractRenderPass(const std::string &id) : AbstractPass(id, false) {
    }

    void AbstractRenderPass::recordDrawSimpleInstanced(const unsigned int vertexCount, const unsigned int instanceCount) const {
        vkCmdDraw(vkCommandBuffer, vertexCount, instanceCount, 0, 0);
    }

    void AbstractRenderPass::recordDrawMesh(const MeshInstance *instance, const unsigned int instanceCount) const {
        if (instance == nullptr || instance->dataBuffer == nullptr || instance->indexBuffer == nullptr) return;
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, &instance->dataBuffer->vkBuffer, offsets);
        vkCmdBindIndexBuffer(vkCommandBuffer, instance->indexBuffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(vkCommandBuffer, instance->indexCount, instanceCount, 0, 0, 0);
    }
}
