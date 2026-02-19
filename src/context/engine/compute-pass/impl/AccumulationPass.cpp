#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .setPushConstantsSize(sizeof(AccumulationPushConstant))
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.previousFrameDescriptor.get());
        pipelineInstance = ApplicationContext::Get().pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void AccumulationPass::onSync() {
        syncWriting(ApplicationContext::Get().coreTextures.currentFrame->vkImage);
        if (initialized) {
            recordPushConstant(&pushConstant);
            recordImageDispatch(ApplicationContext::Get().coreTextures.currentFrame, 8, 8);
        }
        initialized = true;
        pushConstant.previousFrameProjView = ApplicationContext::Get().engineContext.getGlobalDataUBO().projView;
    }
} // Metal
