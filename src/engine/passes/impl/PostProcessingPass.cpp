#include "../render-pass/impl/PostProcessingPass.h"

#include "../../../../context/ApplicationContext.h"

#include "../../dto/PipelineBuilder.h"
#include "../../../editor/enum/EngineResourceIDs.h"


namespace Metal {
    void PostProcessingPass::onInitialize() {
        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .setPushConstantsSize(sizeof(PostProcessingPushConstant))
                .addCombinedImageSamplerBinding(CTX.vulkanContext.vkImageSampler, frame->getResourceAs<TextureInstance>(RID_DENOISED_FRAME)->vkImageView);
        pipelineInstance = CTX.pipelineService.createPipeline(ppPipelineBuilder);
    }

    void PostProcessingPass::onSync() {
        auto &camera = CTX.worldRepository.camera;
        pushConstant.distortionIntensity = camera.distortionIntensity;
        pushConstant.chromaticAberrationIntensity = camera.chromaticAberrationIntensity;
        pushConstant.distortionEnabled = camera.distortionEnabled;
        pushConstant.chromaticAberrationEnabled = camera.chromaticAberrationEnabled;
        pushConstant.vignetteEnabled = camera.vignetteEnabled;
        pushConstant.vignetteStrength = camera.vignetteStrength;

        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }

} // Metal
