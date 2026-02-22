#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .addDescriptorSet(CTX.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.accumulatedFrameDescriptor.get());
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    void AccumulationPass::onSync() {
        syncWriting(CTX.coreTextures.accumulatedFrame->vkImage);
        recordImageDispatch(CTX.coreTextures.accumulatedFrame, 8, 8);
        endWriting(CTX.coreTextures.rawRenderedFrame->vkImage);
    }
} // Metal
