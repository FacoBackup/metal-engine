#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .setPushConstantsSize(sizeof(AccumulationPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.previousFrameDescriptor.get());
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void AccumulationPass::onSync() {
        if (initialized) {
            recordPushConstant(&pushConstant);
            recordImageDispatch(context.coreTextures.currentFrame, 8, 8);
            endWriting(context.coreTextures.currentFrame->vkImage);
        }
        initialized = true;
        pushConstant.previousFrameProjView = context.engineContext.getGlobalDataUBO().projView;
    }
} // Metal
