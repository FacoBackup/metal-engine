#include "IconsPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void IconsPass::onInitialize() {
        PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                    "postProcessingFBO",
                    "QUAD.vert",
                    "tools/Icon.frag"
                )
                .addResourceBinding("globalData")
                .addResourceBinding("lightBuffer")
                .setBlendEnabled();
        pipelineInstance = CTX.pipelineService.createPipeline(iconPipelineBuilder);
    }

    bool IconsPass::shouldRun() {
        return CTX.isDebugMode() && CTX.editorRepository.showIcons;
    }

    void IconsPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
