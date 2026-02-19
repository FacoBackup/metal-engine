#include "IconsPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void IconsPass::onInitialize() {
        PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                    ApplicationContext::Get().coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/Icon.frag"
                )
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.lightData.get());
        pipelineInstance = ApplicationContext::Get().pipelineService.createPipeline(iconPipelineBuilder);
    }

    bool IconsPass::shouldRun() {
        return ApplicationContext::Get().isDebugMode() && ApplicationContext::Get().editorRepository.showIcons;
    }

    void IconsPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
