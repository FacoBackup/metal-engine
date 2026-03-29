#include "CameraService.h"

#include "../dto/Camera.h"
#include "../../ApplicationContext.h"
#include "../EngineContext.h"
#include "../repository/WorldRepository.h"
#include "../repository/RuntimeRepository.h"
#include "../service/DirtyStateService.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    void CameraService::onInitialize() {
        dirtyStateService->markDirty(DirtyType::Camera);
        eventListener([this](const Event &) {
            dirtyStateService->markDirty(DirtyType::Camera);
        }, "Camera");
    }

    // Per frame
    void CameraService::onSync() {
        camera = &worldRepository->camera;
        if (camera != nullptr) {
            updateAspectRatio();
            if (dirtyStateService->isDirty(DirtyType::Camera)) {
                updateMatrices();
                engineContext->setCameraUpdated(true);
                dirtyStateService->consumeDirtyFlags(DirtyType::Camera);
            }
        }
    }

    void CameraService::updateAspectRatio() const {
        const float prevAspect = camera->aspectRatio;
        camera->aspectRatio = runtimeRepository->viewportW / runtimeRepository->viewportH;
        if (prevAspect != camera->aspectRatio) {
            dirtyStateService->markDirty(DirtyType::Camera);
        }
    }

    void CameraService::updateMatrices() {
        updateProjection();
        updateView();
        camera->projViewMatrix = camera->projectionMatrix * camera->viewMatrix;
        camera->invProjView = glm::inverse(camera->projViewMatrix);
        camera->extractFrustumPlanes(camera->projViewMatrix);
    }

    void CameraService::updateView() {
        createViewMatrix();
        camera->invViewMatrix = inverse(camera->viewMatrix);
    }

    void CameraService::updateProjection() const {
        if (camera->isOrthographic) {
            camera->projectionMatrix = glm::ortho(-camera->orthographicProjectionSize,
                                                  camera->orthographicProjectionSize,
                                                  -camera->orthographicProjectionSize / camera->aspectRatio,
                                                  camera->orthographicProjectionSize / camera->aspectRatio,
                                                  camera->zNear, camera->zFar);
        } else {
            camera->projectionMatrix = glm::perspective(camera->fov * Util::TO_RADIANS, camera->aspectRatio,
                                                        camera->zNear,
                                                        camera->zFar);
        }
        camera->projectionMatrix[1][1] *= -1;
        camera->invProjectionMatrix = glm::inverse(camera->projectionMatrix);
    }

    void CameraService::createViewMatrix() {
        const float cosPitch = std::cos(camera->pitch);
        const float sinPitch = std::sin(camera->pitch);
        const float cosYaw = std::cos(camera->yaw);
        const float sinYaw = std::sin(camera->yaw);

        xAxis = glm::vec3(cosYaw, 0.0f, -sinYaw);
        yAxis = glm::vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
        zAxis = glm::vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

        camera->viewMatrix = glm::mat4x4(
            xAxis.x, yAxis.x, zAxis.x, 0.0f,
            xAxis.y, yAxis.y, zAxis.y, 0.0f,
            xAxis.z, yAxis.z, zAxis.z, 0.0f,
            -dot(xAxis, camera->position),
            -dot(yAxis, camera->position),
            -dot(zAxis, camera->position), 1.0f);
    }
}
