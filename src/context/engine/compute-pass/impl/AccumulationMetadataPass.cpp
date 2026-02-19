#include "AccumulationMetadataPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void AccumulationMetadataPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("AccumulationMetadata.comp")
                .setPushConstantsSize(sizeof(DenoiserPushConstant))
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.previousFrameDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.previousFrameMetadataDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.gBufferNormal.get());
        pipelineInstance = ApplicationContext::Get().pipelineService.createPipeline(builder);
    }

    void AccumulationMetadataPass::onSync() {
        syncWriting(ApplicationContext::Get().coreTextures.currentFrame->vkImage);
        pushConstant.diffWeight = ApplicationContext::Get().engineRepository.denoiserDiffWeight;
        recordPushConstant(&pushConstant);
        recordImageDispatch(ApplicationContext::Get().coreTextures.currentFrame, 8, 8);
        endWriting(ApplicationContext::Get().coreTextures.currentFrame->vkImage);
    }
} // Metal
