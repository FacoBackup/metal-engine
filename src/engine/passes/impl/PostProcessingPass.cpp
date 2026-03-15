#include "PostProcessingPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../service/PipelineService.h"
#include "../../../core/vulkan/VulkanContext.h"
#include "../../../engine/repository/WorldRepository.h"
#include "../../../editor/enum/EngineResourceIDs.h"
#include "../../frame-builder/EngineFrame.h"
#include "../../resource/TextureInstance.h"


namespace Metal {
    void PostProcessingPass::onInitialize() {
        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .setPushConstantsSize(sizeof(PostProcessingPushConstant))
                .addCombinedImageSamplerBinding(vulkanContext->vkImageSampler, frame->getResourceAs<TextureInstance>(RID_DENOISED_FRAME)->vkImageView);
        pipelineInstance = pipelineService->createPipeline(ppPipelineBuilder);
    }

    void PostProcessingPass::onSync() {
        auto &camera = worldRepository->camera;
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
