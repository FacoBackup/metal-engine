#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .addResourceBinding(CTX.coreBuffers.globalData)
                .addResourceBinding(CTX.coreTextures.rawRenderedFrame->vkImageView)
                .addResourceBinding(CTX.coreTextures.accumulatedFrame->vkImageView);
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    void AccumulationPass::onSync() {
        syncWriting(CTX.coreTextures.accumulatedFrame->vkImage);
        recordImageDispatch(CTX.coreTextures.accumulatedFrame, 8, 8);
        endWriting(CTX.coreTextures.rawRenderedFrame->vkImage);
    }
} // Metal
