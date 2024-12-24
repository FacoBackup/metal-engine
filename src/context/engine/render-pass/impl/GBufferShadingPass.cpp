#include "GBufferShadingPass.h"
#include "../../../../context/ApplicationContext.h"

namespace Metal {
    void GBufferShadingPass::onSync() {
        if (context.isDebugMode()) {
            pushConstant.mode = context.editorRepository.shadingMode;
            recordPushConstant(&pushConstant);
        }
        recordDrawSimpleInstanced(3, 1);
    }

    PipelineInstance *GBufferShadingPass::getPipeline() {
        return context.corePipelines.gBufferShadingPipeline;
    }
} // Metal
