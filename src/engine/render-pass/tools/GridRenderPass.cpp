#include "GridRenderPass.h"

#include "../../../context/ApplicationContext.h"
#include "../../../context/repository/CorePipelines.h"
#include "../../../context/runtime/PipelineInstance.h"

namespace Metal {
    PipelineInstance *GridRenderPass::getPipeline() {
        return context.vulkanContext.corePipelines.gridPipeline;
    }

    bool GridRenderPass::shouldRun() {
        return context.isDebugMode() && context.editorContext.editorRepository.showGrid;
    }

    void GridRenderPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
