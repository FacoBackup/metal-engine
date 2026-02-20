#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .setPushConstantsSize(sizeof(AccumulationPushConstant))
                .addDescriptorSet(CTX.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(CTX.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.previousFrameDescriptor.get());
        pipelineInstance = CTX.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void AccumulationPass::onSync() {
        syncWriting(CTX.coreTextures.currentFrame->vkImage);
        if (initialized) {
            recordPushConstant(&pushConstant);
            recordImageDispatch(CTX.coreTextures.currentFrame, 8, 8);
        }
        initialized = true;
        pushConstant.previousFrameProjView = CTX.engineContext.getGlobalDataUBO().projView;
    }
} // Metal
