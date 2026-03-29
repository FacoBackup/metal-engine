#include "PostProcessingPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../service/PipelineService.h"
#include "../../../core/VulkanContext.h"
#include "engine/repository/WorldRepository.h"
#include "editor/enum/EngineResourceIDs.h"
#include "../../frame-builder/EngineFrame.h"
#include "../../resource/TextureInstance.h"
#include "engine/resource/RenderTargetInstance.h"


namespace Metal {
    void PostProcessingPass::onInitialize() {
        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_RT),
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .setPushConstantsSize(sizeof(PostProcessingPushConstant))
                .addRenderTargetBinding(getScopedResourceId(RID_MOTION_BLUR_RT), 0);
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
