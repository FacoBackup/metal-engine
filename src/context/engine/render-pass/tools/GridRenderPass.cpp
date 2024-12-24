#include "GridRenderPass.h"

#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/pipeline/CorePipelines.h"
#include "../../../../service/pipeline/PipelineInstance.h"

namespace Metal {
    PipelineInstance *GridRenderPass::getPipeline() {
        return context.corePipelines.gridPipeline;
    }

    bool GridRenderPass::shouldRun() {
        return context.isDebugMode() && context.editorRepository.showGrid;
    }

    void GridRenderPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
