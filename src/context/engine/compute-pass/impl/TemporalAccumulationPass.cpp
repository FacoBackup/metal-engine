#include "TemporalAccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../enum/EngineResourceIDs.h"

namespace Metal {
    void TemporalAccumulationPass::onInitialize() {
        PipelineBuilder temporalBuilder = PipelineBuilder::Of("TemporalAccumulation.comp")
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addStorageImageBinding(getScopedResourceId(RID_ACCUMULATED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_TEMPORAL_OUTPUT))
                .addStorageImageBinding(getScopedResourceId(RID_PREVIOUS_DENOISED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_POSITION_INDEX))
                .addStorageImageBinding(getScopedResourceId(RID_PREVIOUS_POSITION_INDEX))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_NORMAL))
                .addStorageImageBinding(getScopedResourceId(RID_PREVIOUS_NORMAL))
                .addStorageImageBinding(getScopedResourceId(RID_VARIANCE_BUFFER));
        pipelineInstance = CTX.pipelineService.createPipeline(temporalBuilder);
    }

    void TemporalAccumulationPass::onSync() {
        auto *temporalOutput = frame->getResourceAs<TextureInstance>(RID_TEMPORAL_OUTPUT);

        startWriting(temporalOutput->vkImage);
        recordImageDispatch(temporalOutput, 8, 8);
        endWriting(temporalOutput->vkImage);
    }
} // Metal
