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
        for (auto &light: context.worldRepository.lights) {
            if (worldRepository.hiddenEntities.contains(light.second.getEntityId())) {
                continue;
            }
            pushConstant.translation = worldRepository.transforms.at(light.first).translation;
            pushConstant.iconColor = context.editorRepository.iconColor;
            pushConstant.imageIndex = 2;
            pushConstant.iconScale = context.editorRepository.iconScale;
            pushConstant.isSelected = context.editorRepository.selected.contains(light.first);
            recordPushConstant(&pushConstant);
            recordDrawSimpleInstanced(4, 1);
        }
    }
} // Metal
