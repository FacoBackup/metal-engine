#include "GizmoPanel.h"
#include "GizmoTransformStrategy.h"
#include "engine/dto/TransformComponent.h"
#include "engine/repository/CameraRepository.h"
#include "ImGuizmo.h"
#include "../../repository/EditorRepository.h"
#include "../../service/SelectionService.h"

namespace Metal {
    GizmoPanel::GizmoPanel(ImVec2 *position, glm::vec2 *size) : size(size), position(position) {
    }

    void GizmoPanel::onSync() {
        if (editorRepository->primitiveSelected == nullptr) {
            editorRepository->isGizmoOver = false;
            editorRepository->isGizmoUsing = false;
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
        ImGuizmo::SetOrthographic(cameraRepository->isOrthographic);
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

        editorRepository->isGizmoUsing = ImGuizmo::IsUsing();
        gizmoStrategy->updateUsingState(editorRepository->isGizmoUsing);

        if (editorRepository->isGizmoUsing) {
            gizmoStrategy->decomposeMatrix(localSelected);
        } else {
            // Ensure endTransaction is called even if decomposeMatrix wasn't called this frame
            gizmoStrategy->updateUsingState(false);
        }

        editorRepository->isGizmoOver = ImGuizmo::IsOver();
    }
} // Metal
