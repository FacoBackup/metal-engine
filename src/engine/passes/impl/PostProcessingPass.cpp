#include "PostProcessingPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../service/PipelineService.h"
#include "../../../core/VulkanContext.h"
#include "engine/repository/WorldRepository.h"
#include "editor/enum/EngineResourceIDs.h"
#include "../../frame-builder/EngineFrame.h"
#include "../../resource/TextureInstance.h"
#include "engine/resource/FrameBufferInstance.h"


namespace Metal {
    void PostProcessingPass::onInitialize() {
        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .setPushConstantsSize(sizeof(PostProcessingPushConstant))
                .addFboBinding(getScopedResourceId(RID_DOF_FBO), 0);
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
