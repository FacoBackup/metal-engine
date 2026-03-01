#include "SpatialFilterPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../enum/EngineResourceIDs.h"

namespace Metal {
    void SpatialFilterPass::onInitialize() {
        PipelineBuilder spatialBuilder = PipelineBuilder::Of("SpatialFilter.comp")
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addStorageImageBinding(getScopedResourceId(RID_TEMPORAL_OUTPUT))
                .addStorageImageBinding(getScopedResourceId(RID_DENOISED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_POSITION_INDEX))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_NORMAL))
                .addStorageImageBinding(getScopedResourceId(RID_VARIANCE_BUFFER));
        pipelineInstance = CTX.pipelineService.createPipeline(spatialBuilder);
    }

    void SpatialFilterPass::onSync() {
        auto *denoisedFrame = frame->getResourceAs<TextureInstance>(RID_DENOISED_FRAME);
        auto *previousDenoisedFrame = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_DENOISED_FRAME);

        startWriting(denoisedFrame->vkImage);
        recordImageDispatch(denoisedFrame, 8, 8);
        endWriting(denoisedFrame->vkImage);

        // Update History
        copyTexture(denoisedFrame, previousDenoisedFrame);
    }
} // Metal
