#include "CameraMovementService.h"

#include <iostream>

#include "../../../context/ApplicationContext.h"
#include "../../repository/camera/Camera.h"

namespace Metal {
    void CameraMovementService::handleInputInternal(Camera &camera) const {
        const auto &runtimeRepository = context.getEngineContext().runtimeRepository;
        const auto &cameraRepository = context.getEngineContext().cameraRepository;

        glm::vec3 forward(
            -std::sin(camera.yaw) * std::cos(camera.pitch),
            std::sin(camera.pitch),
            -std::cos(camera.yaw) * std::cos(camera.pitch)
        );
        glm::vec3 right(
            std::sin(camera.yaw - PI_OVER_2),
            0.0f,
            std::cos(camera.yaw - PI_OVER_2)
        );
        forward = glm::normalize(forward);
        right = glm::normalize(right);

        const float multiplier = 10 * cameraRepository.movementSensitivity *
                                 context.getEngineContext().deltaTime;
        if (runtimeRepository.leftPressed) {
            camera.position += right * multiplier;
            camera.registerChange();
        }
        if (runtimeRepository.rightPressed) {
            camera.position -= right * multiplier;
            camera.registerChange();
        }
        if (runtimeRepository.backwardPressed) {
            if (camera.isOrthographic) {
                camera.orthographicProjectionSize += multiplier;
            } else {
                camera.position -= forward * multiplier;
            }
            camera.registerChange();
        }
        if (runtimeRepository.forwardPressed) {
            if (camera.isOrthographic) {
                camera.orthographicProjectionSize -= multiplier;
            } else {
                camera.position += forward * multiplier;
            }
            camera.registerChange();
        }
    }

    void CameraMovementService::handleMouse(Camera &camera, const bool isFirstMovement) const {
        const auto &cameraRepository = context.getEngineContext().cameraRepository;

        updateDelta(isFirstMovement);

        camera.yaw -= glm::radians(cameraRepository.deltaX);
        if (std::abs(camera.yaw) >= PI_2) {
            float mapped = std::abs(camera.yaw) - PI_2;
            camera.yaw = (camera.yaw < 0.0f ? -mapped : mapped);
        }
        camera.pitch += glm::radians(cameraRepository.deltaY);
        camera.pitch = glm::clamp(camera.pitch, -MIN_MAX_PITCH, MIN_MAX_PITCH);

        camera.registerChange();
    }

    void CameraMovementService::updateDelta(const bool isFirstMovement) const {
        const auto &runtimeRepository = context.getEngineContext().runtimeRepository;
        auto &cameraRepository = context.getEngineContext().cameraRepository;

        const float mouseX = runtimeRepository.mouseX;
        const float mouseY = runtimeRepository.mouseY;

        if (isFirstMovement) {
            cameraRepository.lastMouseX = mouseX;
            cameraRepository.lastMouseY = mouseY;
        }

        cameraRepository.deltaX = (mouseX - cameraRepository.lastMouseX) * cameraRepository.rotationSensitivity *
                                  0.25f;
        cameraRepository.deltaY = (cameraRepository.lastMouseY - mouseY) * cameraRepository.rotationSensitivity *
                                  0.25f;

        cameraRepository.lastMouseX = mouseX;
        cameraRepository.lastMouseY = mouseY;
    }

    void CameraMovementService::handleInput(Camera &camera, const bool isFirstMovement) const {
        handleInputInternal(camera);
        handleMouse(camera, isFirstMovement);
    }

    void CameraMovementService::createViewMatrix(const Camera &camera) {
        auto &cameraRepository = context.getEngineContext().cameraRepository;

        const float cosPitch = std::cos(camera.pitch);
        const float sinPitch = std::sin(camera.pitch);
        const float cosYaw = std::cos(camera.yaw);
        const float sinYaw = std::sin(camera.yaw);

        xAxis = glm::vec3(cosYaw, 0.0f, -sinYaw);
        yAxis = glm::vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
        zAxis = glm::vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

        cameraRepository.viewMatrix = glm::mat4x4(
            xAxis.x, yAxis.x, zAxis.x, 0.0f,
            xAxis.y, yAxis.y, zAxis.y, 0.0f,
            xAxis.z, yAxis.z, zAxis.z, 0.0f,
            -dot(xAxis, camera.position),
            -dot(yAxis, camera.position),
            -dot(zAxis, camera.position), 1.0f);

    }
} // Metal
