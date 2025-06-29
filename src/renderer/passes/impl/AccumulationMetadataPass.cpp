#include "AccumulationMetadataPass.h"
#include "../../../MetalContextProvider.h"
#include "../../data/TextureInstance.h"
#include "../../data/PipelineBuilder.h"

namespace Metal {
    void AccumulationMetadataPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("AccumulationMetadata.comp")
                .setPushConstantsSize(sizeof(DenoiserPC))
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.previousFrameDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.previousFrameMetadataDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.currentPositionsDescriptor.get());
        pipelineInstance = SINGLETONS.pipelineService.createPipeline(builder);
    }

    void AccumulationMetadataPass::onSync() {
        pushConstant.diffWeight = SINGLETONS.engineRepository.denoiserDiffWeight;
        recordPushConstant(&pushConstant);
        recordImageDispatch(SINGLETONS.coreTextures.currentFrame, 8, 8);
        endWriting(SINGLETONS.coreTextures.currentFrame->vkImage);
    }
} // Metal
