#include "DepthOfFieldPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../service/PipelineService.h"
#include "../../../core/VulkanContext.h"
#include "engine/repository/EngineRepository.h"
#include "editor/enum/EngineResourceIDs.h"
#include "../../frame-builder/EngineFrame.h"
#include "../../resource/TextureInstance.h"

namespace Metal {
    void DepthOfFieldPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_RT),
                    "QUAD.vert",
                    "DepthOfField.frag"
                )
                .setPushConstantsSize(sizeof(DepthOfFieldPushConstant))
                .addStorageImageBinding(getScopedResourceId(RID_ACCUMULATED_FRAME))
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_RENDER_INDEX_DEPTH);
        
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    void DepthOfFieldPass::onSync() {
        pushConstant.focusDistance = engineRepository->dofFocusDistance;
        pushConstant.aperture = engineRepository->dofAperture;
        pushConstant.focalLength = engineRepository->dofFocalLength;
        pushConstant.enabled = engineRepository->dofEnabled;

        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
}
