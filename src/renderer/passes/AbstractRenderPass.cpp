#include "AbstractRenderPass.h"
#include "../../MetalContextProvider.h"
#include "../data/PipelineInstance.h"

namespace Metal {
    AbstractRenderPass::AbstractRenderPass() : AbstractPass(false) {
    }

    void AbstractRenderPass::recordDrawSimpleInstanced(const unsigned int vertexCount, const unsigned int instanceCount) const {
        vkCmdDraw(vkCommandBuffer, vertexCount, instanceCount, 0, 0);
    }

}
