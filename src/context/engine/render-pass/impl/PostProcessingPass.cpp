#include "PostProcessingPass.h"

#include "../../../../context/ApplicationContext.h"

#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../enum/EngineResourceIDs.h"


namespace Metal {
    void PostProcessingPass::onInitialize() {
        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .setPushConstantsSize(sizeof(PostProcessingPushConstant))
                .addCombinedImageSamplerBinding(CTX.vulkanContext.vkImageSampler, frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME)->vkImageView);
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
