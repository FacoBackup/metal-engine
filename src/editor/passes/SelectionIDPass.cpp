#include "SelectionIDPass.h"
#include "engine/dto/TransformComponent.h"
#include "engine/dto/StaticGeometryComponent.h"
#include "engine/dto/InstancedGeometryComponent.h"
#include "engine/dto/AnimatedGeometryComponent.h"
#include "../dto/SelectionOutlinePushConstant.h"
#include "engine/dto/PipelineBuilder.h"
#include "engine/service/PipelineService.h"
#include "../repository/EditorRepository.h"
#include "engine/repository/WorldRepository.h"
#include "engine/service/MeshService.h"
#include "../enum/EngineResourceIDs.h"

namespace Metal {
    void SelectionIDPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                   getScopedResourceId(RID_SELECTION_FBO),
                    "tools/SelectionID.vert",
                    "tools/SelectionID.frag"
                )
                .enablePrimitiveRendering()
                .setCullMode(VK_CULL_MODE_NONE)
                .setPushConstantsSize(sizeof(SelectionOutlinePushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA));
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool SelectionIDPass::shouldRun() {
        return !editorRepository->isPlaying && !editorRepository->selected.empty();
    }

    void SelectionIDPass::onSync() {
        for (auto const& [entityId, selected] : editorRepository->selected) {
            if (!selected) {
                continue;
            }
            if (worldRepository->hiddenEntities.contains(entityId)) {
                continue;
            }
            if (!worldRepository->registry.all_of<TransformComponent>(entityId)) {
                continue;
            }

            std::string meshId;
            unsigned int renderIndex = 0;

            if (worldRepository->registry.all_of<StaticGeometryComponent>(entityId)) {
                const auto &mesh = worldRepository->registry.get<StaticGeometryComponent>(entityId);
                meshId = mesh.meshId;
                renderIndex = mesh.renderIndex;
            } else if (worldRepository->registry.all_of<InstancedGeometryComponent>(entityId)) {
                const auto &mesh = worldRepository->registry.get<InstancedGeometryComponent>(entityId);
                meshId = mesh.meshId;
                renderIndex = mesh.renderIndex;
            } else if (worldRepository->registry.all_of<AnimatedGeometryComponent>(entityId)) {
                const auto &mesh = worldRepository->registry.get<AnimatedGeometryComponent>(entityId);
                meshId = mesh.meshId;
                renderIndex = mesh.renderIndex;
            }

            if (meshId.empty()) {
                continue;
            }

            const auto *meshInstance = meshService->stream(meshId);
            if (!meshInstance) {
                continue;
            }

            pushConstant.model = worldRepository->registry.get<TransformComponent>(entityId).model;
            pushConstant.selectionColor = glm::vec4(editorRepository->selectionColor, editorRepository->selectionOutlineThickness);
            pushConstant.renderIndex = renderIndex;

            recordPushConstant(&pushConstant);
            recordDrawMesh(meshInstance);
        }
    }
} // Metal
