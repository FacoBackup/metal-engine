#include "AbstractRenderPass.h"
#include "../../../context/ApplicationContext.h"
#include "../../../service/pipeline/PipelineInstance.h"

namespace Metal {
    AbstractRenderPass::AbstractRenderPass(ApplicationContext &context) : AbstractPass(context, false) {
    }

    void AbstractRenderPass::recordDrawSimpleInstanced(const uint32_t vertexCount, const uint32_t instanceCount) const {
        vkCmdDraw(vkCommandBuffer, vertexCount, instanceCount, 0, 0);
    }

}
