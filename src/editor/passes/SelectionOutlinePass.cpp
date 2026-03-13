#include "SelectionOutlinePass.h"

#include "../../ApplicationContext.h"
#include "../../engine/dto/TransformComponent.h"
#include "../dto/SelectedDotPushConstant.h"
#include "../../engine/dto/PipelineBuilder.h"
#include "../enum/EngineResourceIDs.h"

namespace Metal {
    void SelectionOutlinePass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "QUAD.vert",
                    "tools/SelectedDot.frag"
                )
                .setBlendEnabled()
                .setPushConstantsSize(sizeof(SelectedDotPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addFboBinding(getScopedResourceId(RID_SELECTION_FBO), 0);
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    bool SelectionOutlinePass::shouldRun() {
        return !CTX.editorRepository.selected.empty();
    }

    void SelectionOutlinePass::onSync() {

        pushConstant.selectionColor.x = CTX.editorRepository.selectionColor.x;
        pushConstant.selectionColor.y = CTX.editorRepository.selectionColor.y;
        pushConstant.selectionColor.z = CTX.editorRepository.selectionColor.z;
        pushConstant.selectionColor.w = CTX.editorRepository.selectionOutlineThickness;

        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
