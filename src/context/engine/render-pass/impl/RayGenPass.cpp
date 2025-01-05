#include "RayGenPass.h"

#include "../../../ApplicationContext.h"

namespace Metal {
    void RayGenPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }

    PipelineInstance * RayGenPass::getPipeline() {
        return context.corePipelines.rayGenPipeline;
    }
} // Metal
