#include "IconsPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../../enum/EngineResourceIDs.h"

namespace Metal {
    void IconsPass::onInitialize() {
        PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "tools/Icon.frag"
                )
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addBufferBinding(getScopedResourceId(RID_LIGHT_BUFFER))
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
