#include "GBufferShadingPass.h"
#include "../../../../context/ApplicationContext.h"

namespace Metal {
    void GBufferShadingPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }

    PipelineInstance *GBufferShadingPass::getPipeline() {
        return context.corePipelines.shadingPipeline;
    }
} // Metal
