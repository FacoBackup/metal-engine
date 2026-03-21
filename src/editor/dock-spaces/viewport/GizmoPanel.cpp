#include "GizmoPanel.h"
#include "GizmoTransformStrategy.h"
#include "../../../ApplicationContext.h"
#include "../../../engine/dto/TransformComponent.h"
#include "../../../engine/dto/Camera.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "../../repository/EditorRepository.h"
#include "../../../engine/repository/WorldRepository.h"
#include "../../service/SelectionService.h"
#include "../../service/HistoryService.h"

namespace Metal {
    GizmoPanel::GizmoPanel(ImVec2 *position, glm::vec2 *size) : size(size), position(position) {
    }

    GizmoPanel::~GizmoPanel() {
        delete gizmoStrategy;
    }

    void GizmoPanel::onInitialize() {
        gizmoStrategy = new GizmoTransformStrategy(historyService, editorRepository, worldRepository);
    }

    void GizmoPanel::onSync() {
        if (editorRepository->primitiveSelected == nullptr) {
            isGizmoOver = false;
            localSelected = nullptr;
            if (editorRepository->mainSelection != EMPTY_ENTITY) {
                selectionService->updatePrimitiveSelected();
            }
            return;
        }

        if (editorRepository->primitiveSelected != localSelected) {
            localSelected = editorRepository->primitiveSelected;
            gizmoStrategy->updateCache(localSelected);
        }

        gizmoStrategy->recomposeMatrix();
        ImGuizmo::SetOrthographic(worldRepository->camera.isOrthographic);
        ImGuizmo::SetDrawlist();
        ImVec2 viewportMin = ImGui::GetItemRectMin();
        ImVec2 viewportSize = ImGui::GetItemRectSize();
        ImGuizmo::SetRect(viewportMin.x, viewportMin.y, viewportSize.x, viewportSize.y);
        Manipulate(
            gizmoStrategy->getViewMatrixCache(),
            gizmoStrategy->getProjectionMatrixCache(),
            editorRepository->gizmoType,
            editorRepository->gizmoMode,
            gizmoStrategy->getCacheMatrix(),
            nullptr,
            gizmoStrategy->getSnapValues());

        gizmoStrategy->updateUsingState(ImGuizmo::IsUsing());

        if (ImGuizmo::IsUsing()) {
            gizmoStrategy->decomposeMatrix(localSelected);
        }

        isGizmoOver = ImGuizmo::IsOver();
    }
} // Metal
