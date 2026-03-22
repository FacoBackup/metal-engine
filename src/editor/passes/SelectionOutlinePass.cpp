#include "SelectionOutlinePass.h"
#include "../../ApplicationContext.h"
#include "../../engine/dto/TransformComponent.h"
#include "../dto/SelectionOutlinePushConstant.h"
#include "../../engine/dto/PipelineBuilder.h"
#include "../../engine/service/PipelineService.h"
#include "../repository/EditorRepository.h"
#include "../enum/EngineResourceIDs.h"

namespace Metal {
    void SelectionOutlinePass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "tools/SelectionOutline.frag"
                )
                .enableBlending()
                .setPushConstantsSize(sizeof(SelectionOutlinePushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addFboBinding(getScopedResourceId(RID_SELECTION_FBO), 0);
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool SelectionOutlinePass::shouldRun() {
        return !editorRepository->isPlaying && !editorRepository->selected.empty();
    }

    void SelectionOutlinePass::onSync() {
        pushConstant.selectionColor.x = editorRepository->selectionColor.x;
        pushConstant.selectionColor.y = editorRepository->selectionColor.y;
        pushConstant.selectionColor.z = editorRepository->selectionColor.z;
        pushConstant.selectionColor.w = editorRepository->selectionOutlineThickness;

        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
