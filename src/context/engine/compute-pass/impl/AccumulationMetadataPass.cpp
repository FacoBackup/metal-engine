#include "AccumulationMetadataPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void AccumulationMetadataPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("AccumulationMetadata.comp")
                .setPushConstantsSize(sizeof(DenoiserPushConstant))
                .addDescriptorSet(CTX.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.previousFrameDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.previousFrameMetadataDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(CTX.coreDescriptorSets.gBufferNormal.get());
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    void AccumulationMetadataPass::onSync() {
        syncWriting(CTX.coreTextures.currentFrame->vkImage);
        pushConstant.diffWeight = CTX.engineRepository.denoiserDiffWeight;
        recordPushConstant(&pushConstant);
        recordImageDispatch(CTX.coreTextures.currentFrame, 8, 8);
        endWriting(CTX.coreTextures.currentFrame->vkImage);
    }
} // Metal
