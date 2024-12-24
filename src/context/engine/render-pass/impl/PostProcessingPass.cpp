#include "PostProcessingPass.h"

#include "../../../../context/ApplicationContext.h"

namespace Metal {
    void PostProcessingPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }

    PipelineInstance *PostProcessingPass::getPipeline() {
        return context.corePipelines.postProcessingPipeline;
    }
} // Metal
