#include "IconsPass.h"

#include "../../../../MetalContextProvider.h"
#include "../../../data/PipelineBuilder.h"

namespace Metal {
    void IconsPass::onInitialize() {
        PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                    SINGLETONS.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/Icon.frag"
                )
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.lightsBuffer.get());
        pipelineInstance = SINGLETONS.pipelineService.createPipeline(iconPipelineBuilder);
    }

    bool IconsPass::shouldRun() {
        return ContextProvider::Get().isDebugMode() && SINGLETONS.editorRepository.showIcons;
    }

    void IconsPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
