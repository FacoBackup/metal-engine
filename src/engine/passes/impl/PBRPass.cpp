#include "PBRPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../service/PipelineService.h"
#include "../../../core/VulkanContext.h"
#include "engine/repository/WorldRepository.h"
#include "editor/enum/EngineResourceIDs.h"
#include "../../frame-builder/EngineFrame.h"
#include "../../resource/TextureInstance.h"
#include "engine/repository/EngineRepository.h"

namespace Metal {
    void PBRPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_RT),
                    "QUAD.vert",
                    "PBR.frag"
                )
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_ALBEDO_EMISSIVE)
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_ROUGHNESS_METALLIC)
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_RENDER_INDEX_DEPTH)
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_NORMAL)
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA));
        
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool PBRPass::shouldRun() {
        return !engineRepository->isRayTracingEnabled;
    }

    void PBRPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
}
