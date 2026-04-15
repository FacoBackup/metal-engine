#include "MotionBlurPass.h"
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
    void MotionBlurPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_MOTION_BLUR_RT),
                    "QUAD.vert",
                    "MotionBlur.frag"
                )
                .setPushConstantsSize(sizeof(MotionBlurPushConstant))
                .addRenderTargetBinding(getScopedResourceId(RID_PBR_RT), 0)
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_RENDER_INDEX_DEPTH)
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_MOTION_VECTOR);

        pipelineInstance = pipelineService->createPipeline(builder);
    }

    void MotionBlurPass::onSync() {
        pushConstant.motionBlurEnabled = cameraRepository->motionBlurEnabled;
        pushConstant.motionBlurVelocityScale = cameraRepository->motionBlurVelocityScale;
        pushConstant.motionBlurMaxSamples = cameraRepository->motionBlurMaxSamples;

        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
