#include "CameraService.h"

#include "../repository/CameraRepository.h"
#include "../../ApplicationContext.h"
#include "../EngineContext.h"
#include "../repository/WorldRepository.h"
#include "../repository/RuntimeRepository.h"
#include "../service/DirtyStateService.h"
#include "editor/dto/FieldModificationEvent.h"
#include "engine/dto/CameraComponent.h"
#include "engine/dto/TransformComponent.h"

namespace Metal {
    void CameraService::onInitialize() {
        dirtyStateService->markDirty(DirtyType::Camera);
        eventListener([this](const Event &) {
            dirtyStateService->markDirty(DirtyType::Camera);
        }, "Camera");

        eventListener([this](const Event &e) {
            isPlaying = e.key == "Play";
        }, "Play", "Stop");
    }

    // Per frame
    void CameraService::onSync() {
        if (isPlaying) {
            handleCamera();
        }
        updateAspectRatio();
        if (dirtyStateService->isDirty(DirtyType::Camera)) {
            updateMatrices();
            engineContext->setCameraUpdated(true);
        }
    }

    void CameraService::updateAspectRatio() {
        const float prevAspect = cameraRepository->aspectRatio;
        cameraRepository->aspectRatio = runtimeRepository->viewportW / runtimeRepository->viewportH;
        if (prevAspect != cameraRepository->aspectRatio) {
            dirtyStateService->markDirty(DirtyType::Camera);
        }
    }

    void CameraService::updateMatrices() {
        updateProjection();
        updateView();
        cameraRepository->projViewMatrix = cameraRepository->projectionMatrix * cameraRepository->viewMatrix;
        cameraRepository->invProjView = glm::inverse(cameraRepository->projViewMatrix);
        cameraRepository->extractFrustumPlanes(cameraRepository->projViewMatrix);
    }

    void CameraService::updateView() {
        createViewMatrix();
        cameraRepository->invViewMatrix = inverse(cameraRepository->viewMatrix);
    }

    void CameraService::updateProjection() {
        if (cameraRepository->isOrthographic) {
            cameraRepository->projectionMatrix = glm::ortho(-cameraRepository->orthographicProjectionSize,
                                                            cameraRepository->orthographicProjectionSize,
                                                            -cameraRepository->orthographicProjectionSize /
                                                            cameraRepository->aspectRatio,
                                                            cameraRepository->orthographicProjectionSize /
                                                            cameraRepository->aspectRatio,
                                                            cameraRepository->zNear, cameraRepository->zFar);
        } else {
            cameraRepository->projectionMatrix = glm::perspective(cameraRepository->fov * Util::TO_RADIANS,
                                                                  cameraRepository->aspectRatio,
                                                                  cameraRepository->zNear,
                                                                  cameraRepository->zFar);
        }
        cameraRepository->projectionMatrix[1][1] *= -1;
        cameraRepository->invProjectionMatrix = glm::inverse(cameraRepository->projectionMatrix);
    }

    void CameraService::createViewMatrix() {
        const float cosPitch = std::cos(cameraRepository->pitch);
        const float sinPitch = std::sin(cameraRepository->pitch);
        const float cosYaw = std::cos(cameraRepository->yaw);
        const float sinYaw = std::sin(cameraRepository->yaw);

        xAxis = glm::vec3(cosYaw, 0.0f, -sinYaw);
        yAxis = glm::vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
        zAxis = glm::vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

        cameraRepository->viewMatrix = glm::mat4x4(
            xAxis.x, yAxis.x, zAxis.x, 0.0f,
            xAxis.y, yAxis.y, zAxis.y, 0.0f,
            xAxis.z, yAxis.z, zAxis.z, 0.0f,
            -dot(xAxis, cameraRepository->position),
            -dot(yAxis, cameraRepository->position),
            -dot(zAxis, cameraRepository->position), 1.0f);
    }


    void CameraService::updateCameraData(entt::entity entity) {
        if (entity == entt::null) return;
        auto *component = worldRepository->registry.try_get<CameraComponent>(entity);
        auto *transform = worldRepository->registry.try_get<TransformComponent>(entity);
        if (!component || !transform) return;

        cameraRepository->fov = component->fov;
        cameraRepository->zNear = component->zNear;
        cameraRepository->zFar = component->zFar;
        cameraRepository->rotationSensitivity = component->rotationSensitivity;
        cameraRepository->movementSensitivity = component->movementSensitivity;
        cameraRepository->zoomSensitivity = component->zoomSensitivity;
        cameraRepository->motionBlurEnabled = component->motionBlurEnabled;
        cameraRepository->motionBlurVelocityScale = component->motionBlurVelocityScale;
        cameraRepository->motionBlurMaxSamples = component->motionBlurMaxSamples;
        cameraRepository->cameraMotionBlur = component->cameraMotionBlur;
        cameraRepository->dofEnabled = component->dofEnabled;
        cameraRepository->dofFocusDistance = component->dofFocusDistance;
        cameraRepository->dofAperture = component->dofAperture;
        cameraRepository->dofFocalLength = component->dofFocalLength;
        cameraRepository->bloomEnabled = component->bloomEnabled;
        cameraRepository->filmGrain = component->filmGrain;
        cameraRepository->vignetteEnabled = component->vignetteEnabled;
        cameraRepository->chromaticAberrationEnabled = component->chromaticAberrationEnabled;
        cameraRepository->distortionEnabled = component->distortionEnabled;
        cameraRepository->filmGrainStrength = component->filmGrainStrength;
        cameraRepository->vignetteStrength = component->vignetteStrength;
        cameraRepository->bloomThreshold = component->bloomThreshold;
        cameraRepository->bloomQuality = component->bloomQuality;
        cameraRepository->bloomOffset = component->bloomOffset;
        cameraRepository->chromaticAberrationIntensity = component->chromaticAberrationIntensity;
        cameraRepository->distortionIntensity = component->distortionIntensity;
        cameraRepository->isOrthographic = component->isOrthographic;
        cameraRepository->aspectRatio = component->aspectRatio;
        cameraRepository->orthographicProjectionSize = component->orthographicProjectionSize;

        cameraRepository->position = transform->translation;
        cameraRepository->yaw = component->yaw;
        cameraRepository->pitch = component->pitch;

        ApplicationEventContext::dispatch("Camera");
    }

    void CameraService::handleCamera() {
        for (const auto view = worldRepository->registry.view<CameraComponent, TransformComponent>();
             const auto entity:
             view) {
            if (const auto &cameraComp = view.get<CameraComponent>(entity); cameraComp.mainCamera) {
                updateCameraData(entity);
                break;
            }
        }
    }
}
