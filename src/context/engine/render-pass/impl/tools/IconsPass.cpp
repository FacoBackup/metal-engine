#include "IconsPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void IconsPass::onInitialize() {
        PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.shadingFBO,
                    "QUAD.vert",
                    "tools/Icon.frag"
                )
                .setPushConstantsSize(sizeof(IconPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get());
        pipelineInstance = context.pipelineService.createPipeline(iconPipelineBuilder);
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
