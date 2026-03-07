#include "SelectionIDPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../repository/world/components/TransformComponent.h"
#include "../../../../../dto/push-constant/SelectedDotPushConstant.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../../enum/EngineResourceIDs.h"

namespace Metal {
    void SelectionIDPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                   getScopedResourceId(RID_SELECTION_FBO),
                    "tools/SelectionID.vert",
                    "tools/SelectionID.frag"
                )
                .setPrepareForMesh()
                .setCullMode(VK_CULL_MODE_NONE)
                .setPushConstantsSize(sizeof(SelectedDotPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA));
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    bool SelectionIDPass::shouldRun() {
        return !CTX.editorRepository.selected.empty();
    }

    void SelectionIDPass::onSync() {
        auto &worldRepository = CTX.worldRepository;
        for (auto const& [entityId, selected] : CTX.editorRepository.selected) {
            if (!selected) {
                continue;
            }
            if (!worldRepository.registry.all_of<PrimitiveComponent>(entityId) || !worldRepository.registry.all_of<TransformComponent>(entityId)) {
                continue;
            }

            const auto &mesh = worldRepository.registry.get<PrimitiveComponent>(entityId);
            if (mesh.meshId.empty()) {
                continue;
            }
            if (worldRepository.hiddenEntities.contains(entityId)) {
                continue;
            }

            const auto *meshInstance = CTX.meshService.stream(mesh.meshId);
            if (!meshInstance) {
                continue;
            }

            pushConstant.model = worldRepository.registry.get<TransformComponent>(entityId).model;
            pushConstant.selectionColor = glm::vec4(CTX.editorRepository.selectionColor, CTX.editorRepository.selectionOutlineThickness);
            pushConstant.renderIndex = mesh.renderIndex;

            recordPushConstant(&pushConstant);
            recordDrawMesh(meshInstance);
        }
    }
} // Metal
