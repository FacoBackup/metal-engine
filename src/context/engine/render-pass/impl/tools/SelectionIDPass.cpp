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
        for (const auto &pair: CTX.editorRepository.selected) {
            if (!pair.second) {
                continue;
            }
            const EntityID entityId = pair.first;
            const auto entity = static_cast<entt::entity>(entityId);
            if (!worldRepository.registry.all_of<PrimitiveComponent>(entity) || !worldRepository.registry.all_of<TransformComponent>(entity)) {
                continue;
            }

            const auto &mesh = worldRepository.registry.get<PrimitiveComponent>(entity);
            if (mesh.meshId.empty()) {
                continue;
            }
            if (worldRepository.hiddenEntities.contains(entityId)) {
                continue;
            }

            const auto *meshInstance = CTX.streamingService.streamMesh(mesh.meshId);
            if (!meshInstance) {
                continue;
            }

            pushConstant.model = worldRepository.registry.get<TransformComponent>(entity).model;
            pushConstant.selectionColor.x = CTX.editorRepository.selectionColor.x;
            pushConstant.selectionColor.y = CTX.editorRepository.selectionColor.y;
            pushConstant.selectionColor.z = CTX.editorRepository.selectionColor.z;
            pushConstant.selectionColor.w = CTX.editorRepository.selectionOutlineThickness;
            pushConstant.renderIndex = mesh.renderIndex;

            recordPushConstant(&pushConstant);
            recordDrawMesh(meshInstance);
        }
    }
} // Metal
