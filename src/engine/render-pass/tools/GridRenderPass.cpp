#include "GridRenderPass.h"
#include "../../../context/repository/CorePipelines.h"
#include "../../../context/runtime/PipelineInstance.h"

namespace Metal {
    void GridRenderPass::onSync() {
        pipelines.gridPipeline->startRecording();
        pipelines.gridPipeline->recordDrawSimpleInstanced(3, 1);
        pipelines.gridPipeline->stopRecording();

        registerCommandBuffer(pipelines.gridPipeline);
    }
} // Metal