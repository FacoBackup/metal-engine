#include "GridPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../../enum/EngineResourceIDs.h"

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
                .addStorageImageBinding(getScopedResourceId(RID_RENDER_INDEX_STENCIL));
        pipelineInstance = CTX.pipelineService.createPipeline(gridPipelineBuilder);
    }

    bool GridPass::shouldRun() {
        return CTX.isDebugMode() && CTX.editorRepository.showGrid;
    }

    void GridPass::onSync() {
        pushConstant.scale = CTX.editorRepository.gridScale;
        pushConstant.overlayObjects = CTX.editorRepository.gridOverlayObjects;
        pushConstant.threshold = CTX.editorRepository.gridThreshold;
        pushConstant.thickness = CTX.editorRepository.gridThickness;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
