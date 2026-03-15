#include "SelectionIDPass.h"
#include "../../engine/dto/TransformComponent.h"
#include "../../engine/dto/PrimitiveComponent.h"
#include "../dto/SelectedDotPushConstant.h"
#include "../../engine/dto/PipelineBuilder.h"
#include "../../engine/service/PipelineService.h"
#include "../repository/EditorRepository.h"
#include "../../engine/repository/WorldRepository.h"
#include "../../engine/service/MeshService.h"
#include "../enum/EngineResourceIDs.h"

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
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool SelectionIDPass::shouldRun() {
        return !editorRepository->selected.empty();
    }

    void SelectionIDPass::onSync() {
        for (auto const& [entityId, selected] : editorRepository->selected) {
            if (!selected) {
                continue;
            }
            if (!worldRepository->registry.all_of<PrimitiveComponent>(entityId) || !worldRepository->registry.all_of<TransformComponent>(entityId)) {
                continue;
            }

            const auto &mesh = worldRepository->registry.get<PrimitiveComponent>(entityId);
            if (mesh.meshId.empty()) {
                continue;
            }
            if (worldRepository->hiddenEntities.contains(entityId)) {
                continue;
            }

            const auto *meshInstance = meshService->stream(mesh.meshId);
            if (!meshInstance) {
                continue;
            }

            pushConstant.model = worldRepository->registry.get<TransformComponent>(entityId).model;
            pushConstant.selectionColor = glm::vec4(editorRepository->selectionColor, editorRepository->selectionOutlineThickness);
            pushConstant.renderIndex = mesh.renderIndex;

            recordPushConstant(&pushConstant);
            recordDrawMesh(meshInstance);
        }
    }
} // Metal
