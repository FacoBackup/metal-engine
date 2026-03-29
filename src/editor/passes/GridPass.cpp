#include "GridPass.h"
#include "engine/dto/PipelineBuilder.h"
#include "engine/service/PipelineService.h"
#include "../repository/EditorRepository.h"
#include "../enum/EngineResourceIDs.h"

namespace Metal {
    void GridPass::onInitialize() {
        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_RT),
                    "QUAD.vert",
                    "tools/Grid.frag"
                )
                .enableBlending()
                .setPushConstantsSize(sizeof(GridPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_RENDER_INDEX_DEPTH);
        pipelineInstance = pipelineService->createPipeline(gridPipelineBuilder);
    }

    bool GridPass::shouldRun() {
        return !editorRepository->isPlaying && editorRepository->showGrid;
    }

    void GridPass::onSync() {
        pushConstant.scale = editorRepository->gridScale;
        pushConstant.overlayObjects = editorRepository->gridOverlayObjects;
        pushConstant.threshold = editorRepository->gridThreshold;
        pushConstant.thickness = editorRepository->gridThickness;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
