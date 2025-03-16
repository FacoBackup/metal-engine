#include "AccumulationMetadataPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void AccumulationMetadataPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("AccumulationMetadata.comp")
                .setPushConstantsSize(sizeof(DenoiserPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.accumulationMetadataDescriptor.get());
        pipelineInstance = context.pipelineService.createPipeline(builder);
    }

    void AccumulationMetadataPass::onSync() {
        pushConstant.diffWeight = context.engineRepository.denoiserDiffWeight;
        recordPushConstant(&pushConstant);
        recordImageDispatch(context.coreTextures.currentFrame, 8, 8);
        endWriting(context.coreTextures.currentFrame->vkImage);
    }
} // Metal
