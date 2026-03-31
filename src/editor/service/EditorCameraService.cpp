#include "EditorCameraService.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/EngineContext.h"
#include "engine/repository/RuntimeRepository.h"
#include "engine/service/DirtyStateService.h"
#include "../../ApplicationEventContext.h"
#include "engine/repository/WorldRepository.h"
#include "engine/repository/CameraRepository.h"

namespace Metal {
    constexpr float PI_OVER_2 = glm::pi<float>() / 2.0f;
    constexpr float PI_2 = glm::pi<float>() * 2.0f;
    constexpr float MIN_MAX_PITCH = glm::radians(89.0f);

    void EditorCameraService::onInitialize() {
        eventListener([this](const Event &) { move(glm::vec3(0, 0, 1)); }, "CameraMoveForward");
        eventListener([this](const Event &) { move(glm::vec3(0, 0, -1)); }, "CameraMoveBackward");
        eventListener([this](const Event &) { move(glm::vec3(-1, 0, 0)); }, "CameraMoveLeft");
        eventListener([this](const Event &) { move(glm::vec3(1, 0, 0)); }, "CameraMoveRight");
        eventListener([this](const Event &) { move(glm::vec3(0, 1, 0)); }, "CameraMoveUp");
        eventListener([this](const Event &) { move(glm::vec3(0, -1, 0)); }, "CameraMoveDown");

        eventListener([this](const Event &event) {
            auto mouseEvent = std::static_pointer_cast<MouseEventPayload>(event.payload);
            rotate(mouseEvent->deltaX, mouseEvent->deltaY);
        }, "CameraRotate");
    }

    void EditorCameraService::move(const glm::vec3 &direction) {
        glm::vec3 forward(
            -std::sin(cameraRepository->yaw) * std::cos(cameraRepository->pitch),
            std::sin(cameraRepository->pitch),
            -std::cos(cameraRepository->yaw) * std::cos(cameraRepository->pitch)
        );
        glm::vec3 right(
            std::sin(cameraRepository->yaw - PI_OVER_2),
            0.0f,
            std::cos(cameraRepository->yaw - PI_OVER_2)
        );
        forward = glm::normalize(forward);
        right = glm::normalize(right);

        const float multiplier = 10 * cameraRepository->movementSensitivity *
                                 engineContext->deltaTime;

        if (direction.z > 0) cameraRepository->position += forward * multiplier;
        if (direction.z < 0) cameraRepository->position -= forward * multiplier;
        if (direction.x < 0) cameraRepository->position += right * multiplier;
        if (direction.x > 0) cameraRepository->position -= right * multiplier;
        if (direction.y > 0) cameraRepository->position += glm::vec3(0, 1, 0) * multiplier;
        if (direction.y < 0) cameraRepository->position -= glm::vec3(0, 1, 0) * multiplier;

        ApplicationEventContext::dispatch("Camera");
    }

    void EditorCameraService::rotate(float deltaX, float deltaY) {
        cameraRepository->yaw -= glm::radians(deltaX * cameraRepository->rotationSensitivity * 0.25f);
        if (std::abs(cameraRepository->yaw) >= PI_2) {
            float mapped = std::abs(cameraRepository->yaw) - PI_2;
            cameraRepository->yaw = (cameraRepository->yaw < 0.0f ? -mapped : mapped);
        }
        cameraRepository->pitch += glm::radians(deltaY * cameraRepository->rotationSensitivity * 0.25f);
        cameraRepository->pitch = glm::clamp(cameraRepository->pitch, -MIN_MAX_PITCH, MIN_MAX_PITCH);

        ApplicationEventContext::dispatch("Camera");
    }
}
