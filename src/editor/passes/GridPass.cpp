#include "GridPass.h"
#include "../../ApplicationContext.h"
#include "../../engine/dto/PipelineBuilder.h"
#include "../../engine/service/PipelineService.h"
#include "../repository/EditorRepository.h"
#include "../enum/EngineResourceIDs.h"

namespace Metal {
    void GridPass::onInitialize() {
        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "tools/Grid.frag"
                )
                .setBlendEnabled()
                .setPushConstantsSize(sizeof(GridPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_POSITION_INDEX));
        pipelineInstance = pipelineService->createPipeline(gridPipelineBuilder);
    }

    bool GridPass::shouldRun() {
        return applicationContext->isDebugMode() && editorRepository->showGrid;
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
