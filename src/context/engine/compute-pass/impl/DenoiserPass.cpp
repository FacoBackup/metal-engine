#include "DenoiserPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void DenoiserPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of("Denoiser.comp")
                .setPushConstantsSize(sizeof(DenoiserPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.previousFrameDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get());
        pipelineInstance = context.pipelineService.createPipeline(builder);
    }

    void DenoiserPass::onSync() {
        // clearTexture(context.coreTextures.previousFrame->vkImage);

        pushConstant.stepWidth = context.engineRepository.denoiserStepWidth;
        pushConstant.normalPhi = context.engineRepository.denoiserNormalPhi;
        recordPushConstant(&pushConstant);
        startWriting(context.coreTextures.currentFrame->vkImage);
        recordImageDispatch(context.coreTextures.currentFrame, 8, 8);
    }
} // Metal
