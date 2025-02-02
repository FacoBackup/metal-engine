#include "IconsPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void IconsPass::onInitialize() {
        PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/Icon.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.lightVolumeData.get());
        pipelineInstance = context.pipelineService.createPipeline(iconPipelineBuilder);
    }

    bool IconsPass::shouldRun() {
        return context.isDebugMode() && context.editorRepository.showIcons;
    }

    void IconsPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
