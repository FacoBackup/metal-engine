#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .addResourceBinding("globalData")
                .addResourceBinding(CTX.textureService.getResource("rawRenderedFrame")->vkImageView)
                .addResourceBinding(CTX.textureService.getResource("accumulatedFrame")->vkImageView);
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    void AccumulationPass::onSync() {
        auto *accumulatedFrame = CTX.textureService.getResource("accumulatedFrame");
        auto *rawRenderedFrame = CTX.textureService.getResource("rawRenderedFrame");
        syncWriting(accumulatedFrame->vkImage);
        recordImageDispatch(accumulatedFrame, 8, 8);
        endWriting(rawRenderedFrame->vkImage);
    }
} // Metal
