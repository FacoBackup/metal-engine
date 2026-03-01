#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../enum/EngineResourceIDs.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .addResourceBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addResourceBinding(frame->getResourceAs<TextureInstance>(RID_RAW_RENDERED_FRAME)->vkImageView)
                .addResourceBinding(frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME)->vkImageView);
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    void AccumulationPass::onSync() {
        auto *accumulatedFrame = frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME);
        auto *rawRenderedFrame = frame->getResourceAs<TextureInstance>(RID_RAW_RENDERED_FRAME);
        syncWriting(accumulatedFrame->vkImage);
        recordImageDispatch(accumulatedFrame, 8, 8);
        endWriting(rawRenderedFrame->vkImage);
    }
} // Metal
