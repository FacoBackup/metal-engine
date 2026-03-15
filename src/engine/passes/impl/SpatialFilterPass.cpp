#include "SpatialFilterPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../resource/TextureInstance.h"
#include "../../service/PipelineService.h"
#include "../../frame-builder/EngineFrame.h"
#include "../../../editor/enum/EngineResourceIDs.h"

namespace Metal {
    void SpatialFilterPass::onInitialize() {
        PipelineBuilder spatialBuilder = PipelineBuilder::Of("SpatialFilter.comp")
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addStorageImageBinding(getScopedResourceId(RID_TEMPORAL_OUTPUT))
                .addStorageImageBinding(getScopedResourceId(RID_DENOISED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_POSITION_INDEX))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_NORMAL));
        pipelineInstance = pipelineService->createPipeline(spatialBuilder);
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
