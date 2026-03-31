#include "PostProcessingPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../service/PipelineService.h"
#include "../../../core/VulkanContext.h"
#include "engine/repository/WorldRepository.h"
#include "editor/enum/EngineResourceIDs.h"
#include "../../frame-builder/EngineFrame.h"
#include "../../resource/TextureInstance.h"
#include "engine/resource/RenderTargetInstance.h"
#include "engine/repository/CameraRepository.h"


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
        pushConstant.distortionIntensity = cameraRepository->distortionIntensity;
        pushConstant.chromaticAberrationIntensity = cameraRepository->chromaticAberrationIntensity;
        pushConstant.distortionEnabled = cameraRepository->distortionEnabled;
        pushConstant.chromaticAberrationEnabled = cameraRepository->chromaticAberrationEnabled;
        pushConstant.vignetteEnabled = cameraRepository->vignetteEnabled;
        pushConstant.vignetteStrength = cameraRepository->vignetteStrength;

        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
