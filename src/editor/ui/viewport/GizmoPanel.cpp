#include "GizmoPanel.h"

#include "../../../context/ApplicationContext.h"
#include "../../../engine/service/world/components/TransformComponent.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Metal {
    GizmoPanel::GizmoPanel(ImVec2 *position, glm::vec2 *size): size(size), position(position) {
    }

    void GizmoPanel::onInitialize() {
        editorRepository = &context->getEditorContext().editorRepository;
    }

    void GizmoPanel::onSync() {
        if (editorRepository->primitiveSelected == nullptr) {
            isGizmoOver = false;
            localSelected = nullptr;
            localChangeId = 0;
            if (editorRepository->mainSelection != EMPTY_ENTITY) {
                context->getEditorContext().selectionService.updatePrimitiveSelected();
            }
            return;
        }

        if (editorRepository->primitiveSelected != localSelected || localSelected->getChangeId() != localChangeId) {
            cacheMatrix = glm::value_ptr(editorRepository->primitiveSelected->model);
            localSelected = editorRepository->primitiveSelected;
            localChangeId = localSelected->getChangeId();
        }

        recomposeMatrix();
        const float snap = getSnapValues();
        ImGuizmo::SetOrthographic(context->getEngineContext().worldRepository.camera.isOrthographic);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(position->x, position->y, size->x, size->y);
        Manipulate(
            viewMatrixCache,
            projectionMatrixCache,
            editorRepository->gizmoType,
            editorRepository->gizmoMode,
            cacheMatrix,
            nullptr,
            &snap);
        if (ImGuizmo::IsUsing()) {
            decomposeMatrix();
        }
        isGizmoOver = ImGuizmo::IsOver();
    }

    float GizmoPanel::getSnapValues() const {
        switch (editorRepository->gizmoType) {
            case ImGuizmo::OPERATION::TRANSLATE: {
                if (editorRepository->gizmoUseSnapTranslate) {
                    return editorRepository->gizmoSnapTranslate;
                }
            }
            case ImGuizmo::OPERATION::ROTATE: {
                if (editorRepository->gizmoUseSnapRotate) {
                    return editorRepository->gizmoSnapRotate;
                }
            }
            case ImGuizmo::OPERATION::SCALE: {
                if (editorRepository->gizmoUseSnapScale) {
                    return editorRepository->gizmoSnapScale;
                }
            }
            default:
                return 0.0f;
        }
    }

    void GizmoPanel::decomposeMatrix() {
        glm::mat4 auxMat4 = glm::make_mat4(cacheMatrix);
        glm::vec3 skew;
        glm::vec4 perspective;

        // Decompose the matrix into its components
        glm::decompose(auxMat4, auxScale, auxRot, auxTranslation, skew, perspective);

        auxTranslation = auxTranslation - localSelected->translation;
        auxScale = auxScale - localSelected->scale;
        auxRot = auxRot - localSelected->rotation;

        localSelected->translation += auxTranslation;
        localSelected->scale += auxScale;
        localSelected->rotation += auxRot;

        localSelected->registerChange();
        localChangeId = localSelected->getChangeId();
        localSelected->forceTransform = true;
    }

    void GizmoPanel::recomposeMatrix() {
        viewMatrixCache = glm::value_ptr(context->getEngineContext().worldRepository.camera.viewMatrix);
        projectionMatrixCache = glm::value_ptr(context->getEngineContext().worldRepository.camera.projectionMatrix);
    }
} // Metal
