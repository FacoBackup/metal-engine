#include "GridPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../repository/pipeline/CorePipelines.h"

namespace Metal {
    PipelineInstance *GridPass::getPipeline() {
        return context.corePipelines.gridPipeline;
    }

    bool GridPass::shouldRun() {
        return context.isDebugMode() && context.editorRepository.showGrid;
    }

    void GridPass::onSync() {
        pushConstant.scale = context.editorRepository.gridScale;
        pushConstant.overlayObjects = context.editorRepository.gridOverlayObjects;
        pushConstant.threshold = context.editorRepository.gridThreshold;
        pushConstant.thickness = context.editorRepository.gridThickness;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
