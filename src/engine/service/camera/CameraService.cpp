#include "CameraService.h"

#include "Camera.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void CameraService::onSync() {
        camera = &context.getEngineContext().worldRepository.camera;
        if (camera != nullptr) {
            updateAspectRatio();
            if (camera->isNotFrozen()) {
                updateMatrices();
                camera->freezeVersion();
            }
        }
    }

    void CameraService::updateAspectRatio() const {
        const auto &runtimeRepository = context.getEngineContext().runtimeRepository;

        const float prevAspect = camera->aspectRatio;
        camera->aspectRatio = runtimeRepository.viewportW / runtimeRepository.viewportH;
        if (prevAspect != camera->aspectRatio) {
            camera->registerChange();
        }
    }

    void CameraService::updateMatrices() {
        updateProjection();
        updateView();
        camera->projViewMatrix = camera->projectionMatrix * camera->viewMatrix;
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
                                                  -camera->zFar, camera->zFar);
        } else {
            camera->projectionMatrix = glm::perspective(camera->fov * Util::TO_RADIANS, camera->aspectRatio,
                                                        camera->zNear,
                                                        camera->zFar);
        }
        camera->projectionMatrix[1][1] *= -1;
        camera->invProjectionMatrix = glm::inverse(camera->projectionMatrix);
    }

    CameraService::CameraService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void CameraService::handleInputInternal() const {
        const auto &runtimeRepository = context.getEngineContext().runtimeRepository;

        glm::vec3 forward(
            -std::sin(camera->yaw) * std::cos(camera->pitch),
            std::sin(camera->pitch),
            -std::cos(camera->yaw) * std::cos(camera->pitch)
        );
        glm::vec3 right(
            std::sin(camera->yaw - PI_OVER_2),
            0.0f,
            std::cos(camera->yaw - PI_OVER_2)
        );
        forward = glm::normalize(forward);
        right = glm::normalize(right);

        const float multiplier = 10 * camera->movementSensitivity *
                                 context.getEngineContext().deltaTime;
        if (runtimeRepository.leftPressed) {
            camera->position += right * multiplier;
            camera->registerChange();
        }
        if (runtimeRepository.rightPressed) {
            camera->position -= right * multiplier;
            camera->registerChange();
        }
        if (runtimeRepository.backwardPressed) {
            if (camera->isOrthographic) {
                camera->orthographicProjectionSize += multiplier;
            } else {
                camera->position -= forward * multiplier;
            }
            camera->registerChange();
        }
        if (runtimeRepository.forwardPressed) {
            if (camera->isOrthographic) {
                camera->orthographicProjectionSize -= multiplier;
            } else {
                camera->position += forward * multiplier;
            }
            camera->registerChange();
        }
    }

    void CameraService::handleMouse(const bool isFirstMovement) const {
        updateDelta(isFirstMovement);

        camera->yaw -= glm::radians(camera->deltaX);
        if (std::abs(camera->yaw) >= PI_2) {
            float mapped = std::abs(camera->yaw) - PI_2;
            camera->yaw = (camera->yaw < 0.0f ? -mapped : mapped);
        }
        camera->pitch += glm::radians(camera->deltaY);
        camera->pitch = glm::clamp(camera->pitch, -MIN_MAX_PITCH, MIN_MAX_PITCH);

        camera->registerChange();
    }

    void CameraService::updateDelta(const bool isFirstMovement) const {
        const auto &runtimeRepository = context.getEngineContext().runtimeRepository;
        const float mouseX = runtimeRepository.mouseX;
        const float mouseY = runtimeRepository.mouseY;

        if (isFirstMovement) {
            camera->lastMouseX = mouseX;
            camera->lastMouseY = mouseY;
        }

        camera->deltaX = (mouseX - camera->lastMouseX) * camera->rotationSensitivity *
                         0.25f;
        camera->deltaY = (camera->lastMouseY - mouseY) * camera->rotationSensitivity *
                         0.25f;

        camera->lastMouseX = mouseX;
        camera->lastMouseY = mouseY;
    }

    void CameraService::handleInput(const bool isFirstMovement) const {
        handleInputInternal();
        handleMouse(isFirstMovement);
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
