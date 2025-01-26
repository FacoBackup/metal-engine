#include "DenoiserPass.h"

#include "../../../../context/ApplicationContext.h"

#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void DenoiserPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    context.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "Denoiser.frag"
                )
                .setPushConstantsSize(sizeof(DenoiserPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.noisyInput.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get());
        pipelineInstance = context.pipelineService.createPipeline(builder);
    }

    void DenoiserPass::onSync() {
        pushConstant.stepWidth = context.engineRepository.denoiserStepWidth;
        pushConstant.normalPhi = context.engineRepository.denoiserNormalPhi;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
