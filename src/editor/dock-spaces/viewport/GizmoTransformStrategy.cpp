#include "GizmoTransformStrategy.h"
#include "../../repository/EditorRepository.h"
#include "../../../engine/repository/WorldRepository.h"
#include "../../../engine/dto/TransformComponent.h"
#include "../../service/HistoryService.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Metal {
    GizmoTransformStrategy::GizmoTransformStrategy(HistoryService *historyService, EditorRepository *editorRepository, WorldRepository *worldRepository)
        : historyService(historyService), editorRepository(editorRepository), worldRepository(worldRepository), translationSnap({1, 1, 1}) {
    }

    void GizmoTransformStrategy::updateCache(TransformComponent *selected, int &localChangeId) {
        if (selected == nullptr) return;

        cacheMatrixMat4 = glm::mat4(selected->model);
        cacheMatrixMat4 = glm::translate(cacheMatrixMat4, selected->gizmoCenter);
        
        float* ptr = glm::value_ptr(cacheMatrixMat4);
        for(int i = 0; i < 16; i++) cacheMatrix[i] = ptr[i];
        
        localChangeId = selected->getChangeId();
    }

    void GizmoTransformStrategy::recomposeMatrix() {
        viewMatrixCache = glm::value_ptr(worldRepository->camera.viewMatrix);
        cacheProjection = worldRepository->camera.projectionMatrix;

        cacheProjection[1][1] *= -1;

        // Convert depth range from [0, 1] (Vulkan) to [-1, 1] (ImGuizmo/OpenGL)
        cacheProjection[2][2] = cacheProjection[2][2] * 2.0f - cacheProjection[2][3];
        cacheProjection[3][2] = cacheProjection[3][2] * 2.0f - cacheProjection[3][3];

        projectionMatrixCache = glm::value_ptr(cacheProjection);
    }

    void GizmoTransformStrategy::decomposeMatrix(TransformComponent *selected, int &localChangeId) {
        if (selected == nullptr) return;

        glm::mat4 auxMat4 = glm::make_mat4(cacheMatrix);

        // Remove gizmo offset using the selected TransformComponent
        auxMat4 = glm::translate(auxMat4, -selected->gizmoCenter);

        glm::vec3 skew;
        glm::vec4 perspective;

        glm::vec3 oldTranslation = selected->translation;
        glm::vec3 oldScale = selected->scale;
        glm::vec3 oldRotationEuler = selected->rotationEuler;

        // Decompose the matrix into its components
        glm::decompose(auxMat4, auxScale, auxRot, auxTranslation, skew, perspective);

        auxTranslation = auxTranslation - selected->translation;
        auxScale = auxScale - selected->scale;
        auxRot = auxRot - selected->rotation;

        selected->translation += auxTranslation;
        selected->scale += auxScale;
        selected->rotation += auxRot;
        selected->rotationEuler = glm::eulerAngles(selected->rotation) * (180.f / glm::pi<float>());

        selected->registerChange();
        localChangeId = selected->getChangeId();
        selected->forceTransform = true;
        selected->onUpdate(nullptr);

        if (oldTranslation != selected->translation) {
            historyService->recordChange(selected, "/Translation", oldTranslation, selected->translation);
        }
        if (oldScale != selected->scale) {
            historyService->recordChange(selected, "/Scale", oldScale, selected->scale);
        }
        if (oldRotationEuler != selected->rotationEuler) {
            historyService->recordChange(selected, "/Rotation", oldRotationEuler, selected->rotationEuler);
        }
    }

    void GizmoTransformStrategy::updateUsingState(bool isUsing) {
        if (isUsing) {
            if (!wasUsing) {
                historyService->startTransaction("Gizmo Transform");
            }
        } else if (wasUsing) {
            historyService->endTransaction();
        }
        wasUsing = isUsing;
    }

    float *GizmoTransformStrategy::getSnapValues() {
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
}
