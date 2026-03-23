#include "CameraService.h"

#include "../dto/Camera.h"
#include "../../ApplicationContext.h"
#include "../EngineContext.h"
#include "../repository/WorldRepository.h"
#include "../repository/RuntimeRepository.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    void CameraService::onInitialize() {
        eventListener([this](const Event &e) {
            dirty = true;
        }, "Camera");

        eventListener([this](const Event &e) { forwardPressed = true; }, "CameraMoveForward");
        eventListener([this](const Event &e) { backwardPressed = true; }, "CameraMoveBackward");
        eventListener([this](const Event &e) { leftPressed = true; }, "CameraMoveLeft");
        eventListener([this](const Event &e) { rightPressed = true; }, "CameraMoveRight");
        eventListener([this](const Event &e) { upPressed = true; }, "CameraMoveUp");
        eventListener([this](const Event &e) { downPressed = true; }, "CameraMoveDown");
    }

    // Per frame
    void CameraService::onSync() {
        camera = &worldRepository->camera;
        if (camera != nullptr) {
            updateAspectRatio();
            if (dirty) {
                updateMatrices();
                engineContext->setCameraUpdated(true);
                dirty = false;
            }
        }
        forwardPressed = false;
        backwardPressed = false;
        leftPressed = false;
        rightPressed = false;
        upPressed = false;
        downPressed = false;
    }

    void CameraService::updateAspectRatio() const {
        const float prevAspect = camera->aspectRatio;
        camera->aspectRatio = runtimeRepository->viewportW / runtimeRepository->viewportH;
        if (prevAspect != camera->aspectRatio) {
            dirty = true;
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

    void CameraService::handleInputInternal() const {
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
                                 engineContext->deltaTime;
        if (leftPressed) {
            camera->position += right * multiplier;
            dirty = true;
        }
        if (rightPressed) {
            camera->position -= right * multiplier;
            dirty = true;
        }
        if (backwardPressed) {
            if (camera->isOrthographic) {
                camera->orthographicProjectionSize += multiplier;
            } else {
                camera->position -= forward * multiplier;
            }
            dirty = true;
        }
        if (forwardPressed) {
            if (camera->isOrthographic) {
                camera->orthographicProjectionSize -= multiplier;
            } else {
                camera->position += forward * multiplier;
            }
            dirty = true;
        }
        if (upPressed) {
            camera->position += glm::vec3(0, 1, 0) * multiplier;
            dirty = true;
        }
        if (downPressed) {
            camera->position -= glm::vec3(0, 1, 0) * multiplier;
            dirty = true;
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

        dirty = true;
    }

    void CameraService::updateDelta(const bool isFirstMovement) const {
        const float mouseX = runtimeRepository->mouseX;
        const float mouseY = runtimeRepository->mouseY;

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
