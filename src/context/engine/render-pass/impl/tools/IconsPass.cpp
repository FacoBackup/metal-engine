#include "IconsPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../repository/pipeline/CorePipelines.h"

namespace Metal {
    PipelineInstance *IconsPass::getPipeline() {
        return context.corePipelines.iconPipeline;
    }

    bool IconsPass::shouldRun() {
        return context.isDebugMode() && context.editorRepository.showGrid;
    }

    void IconsPass::onSync() {
        pushConstant.iconColor = context.editorRepository.iconColor;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
