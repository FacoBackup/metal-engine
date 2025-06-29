#include "GizmoPanel.h"

#include "../../../../MetalContextProvider.h"
#include "../../../../runtime/data/components/TransformComponent.h"
#include "../../../../runtime/data/Camera.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Metal {
    GizmoPanel::GizmoPanel(ImVec2 *position, glm::vec2 *size): size(size), position(position) {
    }

    void GizmoPanel::onInitialize() {
        editorRepository = &SINGLETONS.editorRepository;
    }

    void GizmoPanel::onSync() {
        if (editorRepository->primitiveSelected == nullptr) {
            isGizmoOver = false;
            localSelected = nullptr;
            localChangeId = 0;
            if (editorRepository->mainSelection != EMPTY_ENTITY) {
                SINGLETONS.selectionService.updatePrimitiveSelected();
            }
            return;
        }

        if (editorRepository->primitiveSelected != localSelected || localSelected->getChangeId() != localChangeId) {
            cacheMatrixMat4 = glm::mat4(editorRepository->primitiveSelected->model);
            cacheMatrix = glm::value_ptr(cacheMatrixMat4);
            localSelected = editorRepository->primitiveSelected;
            localChangeId = localSelected->getChangeId();
        }

        recomposeMatrix();
        ImGuizmo::SetOrthographic(SINGLETONS.worldRepository.camera.isOrthographic);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(position->x, position->y, size->x, size->y);
        Manipulate(
            viewMatrixCache,
            projectionMatrixCache,
            editorRepository->gizmoType,
            editorRepository->gizmoMode,
            cacheMatrix,
            nullptr,
            getSnapValues());
        if (ImGuizmo::IsUsing()) {
            decomposeMatrix();
        }
        isGizmoOver = ImGuizmo::IsOver();
    }

    float *GizmoPanel::getSnapValues() {
        switch (editorRepository->gizmoType) {
            case ImGuizmo::OPERATION::TRANSLATE: {
                if (editorRepository->gizmoUseSnapTranslate) {
                    translationSnap[0] = translationSnap[1] = translationSnap[2] = editorRepository->gizmoSnapTranslate;
                    return translationSnap.data();
                }
            }
            case ImGuizmo::OPERATION::ROTATE: {
                if (editorRepository->gizmoUseSnapRotate) {
                    return &editorRepository->gizmoSnapRotate;
                }
            }
            case ImGuizmo::OPERATION::SCALE: {
                if (editorRepository->gizmoUseSnapScale) {
                    return &editorRepository->gizmoSnapScale;
                }
            }
            default:
                return nullptr;
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
        localSelected->rotationEuler = glm::eulerAngles(localSelected->rotation);

        localSelected->registerChange();
        localChangeId = localSelected->getChangeId();
        localSelected->forceTransform = true;
        localSelected->onUpdate(nullptr);
    }

    void GizmoPanel::recomposeMatrix() {
        viewMatrixCache = glm::value_ptr(SINGLETONS.worldRepository.camera.viewMatrix);
        cacheProjection = SINGLETONS.worldRepository.camera.projectionMatrix;
        cacheProjection[1][1] *= -1;
        projectionMatrixCache = glm::value_ptr(cacheProjection);
    }
} // Metal
