#include "GizmoPanel.h"

#include "../../../ApplicationContext.h"
#include "../../../engine/dto/TransformComponent.h"
#include "../../../engine/dto/Camera.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Metal {
    GizmoPanel::GizmoPanel(ImVec2 *position, glm::vec2 *size): size(size), position(position) {
    }

    void GizmoPanel::onInitialize() {
        editorRepository = &applicationContext->editorRepository;
    }

    void GizmoPanel::onSync() {
        if (editorRepository->primitiveSelected == nullptr) {
            isGizmoOver = false;
            localSelected = nullptr;
            localChangeId = 0;
            if (editorRepository->mainSelection != EMPTY_ENTITY) {
                applicationContext->selectionService.updatePrimitiveSelected();
            }
            return;
        }

        if (editorRepository->primitiveSelected != localSelected || localSelected->getChangeId() != localChangeId) {
            cacheMatrixMat4 = glm::mat4(editorRepository->primitiveSelected->model);
            
            if (editorRepository->primitiveSelected != nullptr) {
                cacheMatrixMat4 = glm::translate(cacheMatrixMat4, editorRepository->primitiveSelected->gizmoCenter);
            }

            cacheMatrix = glm::value_ptr(cacheMatrixMat4);
            localSelected = editorRepository->primitiveSelected;
            localChangeId = localSelected->getChangeId();
        }

        recomposeMatrix();
        ImGuizmo::SetOrthographic(applicationContext->worldRepository.camera.isOrthographic);
        ImGuizmo::SetDrawlist();
        ImVec2 viewportMin = ImGui::GetItemRectMin();
        ImVec2 viewportSize = ImGui::GetItemRectSize();
        ImGuizmo::SetRect(viewportMin.x, viewportMin.y, viewportSize.x, viewportSize.y);
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

        // Remove gizmo offset using the selected TransformComponent
        if (localSelected != nullptr) {
            auxMat4 = glm::translate(auxMat4, -localSelected->gizmoCenter);
        }

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
        viewMatrixCache = glm::value_ptr(applicationContext->worldRepository.camera.viewMatrix);
        cacheProjection = applicationContext->worldRepository.camera.projectionMatrix;

        cacheProjection[1][1] *= -1;

        // Convert depth range from [0, 1] (Vulkan) to [-1, 1] (ImGuizmo/OpenGL)
        cacheProjection[2][2] = cacheProjection[2][2] * 2.0f - cacheProjection[2][3];
        cacheProjection[3][2] = cacheProjection[3][2] * 2.0f - cacheProjection[3][3];

        projectionMatrixCache = glm::value_ptr(cacheProjection);
    }
} // Metal
