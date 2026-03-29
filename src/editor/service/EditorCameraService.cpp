#include "EditorCameraService.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/EngineContext.h"
#include "engine/repository/RuntimeRepository.h"
#include "engine/service/DirtyStateService.h"
#include "editor/repository/EditorCameraRepository.h"
#include "../../ApplicationEventContext.h"

namespace Metal {
    constexpr float PI_OVER_2 = glm::pi<float>() / 2.0f;
    constexpr float PI_2 = glm::pi<float>() * 2.0f;
    constexpr float MIN_MAX_PITCH = glm::radians(89.0f);

    std::vector<Dependency> EditorCameraService::getDependencies() {
        return {
            {"EngineContext", &engineContext},
            {"EditorCameraRepository", &editorCameraRepository},
            {"RuntimeRepository", &runtimeRepository},
            {"DirtyStateService", &dirtyStateService}
        };
    }

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
            -std::sin(editorCameraRepository->yaw) * std::cos(editorCameraRepository->pitch),
            std::sin(editorCameraRepository->pitch),
            -std::cos(editorCameraRepository->yaw) * std::cos(editorCameraRepository->pitch)
        );
        glm::vec3 right(
            std::sin(editorCameraRepository->yaw - PI_OVER_2),
            0.0f,
            std::cos(editorCameraRepository->yaw - PI_OVER_2)
        );
        forward = glm::normalize(forward);
        right = glm::normalize(right);

        const float multiplier = 10 * editorCameraRepository->movementSensitivity *
                                 engineContext->deltaTime;

        if (direction.z > 0) editorCameraRepository->position += forward * multiplier;
        if (direction.z < 0) editorCameraRepository->position -= forward * multiplier;
        if (direction.x < 0) editorCameraRepository->position += right * multiplier;
        if (direction.x > 0) editorCameraRepository->position -= right * multiplier;
        if (direction.y > 0) editorCameraRepository->position += glm::vec3(0, 1, 0) * multiplier;
        if (direction.y < 0) editorCameraRepository->position -= glm::vec3(0, 1, 0) * multiplier;

        ApplicationEventContext::dispatch("Camera");
    }

    void EditorCameraService::rotate(float deltaX, float deltaY) {
        editorCameraRepository->yaw -= glm::radians(deltaX * editorCameraRepository->rotationSensitivity * 0.25f);
        if (std::abs(editorCameraRepository->yaw) >= PI_2) {
            float mapped = std::abs(editorCameraRepository->yaw) - PI_2;
            editorCameraRepository->yaw = (editorCameraRepository->yaw < 0.0f ? -mapped : mapped);
        }
        editorCameraRepository->pitch += glm::radians(deltaY * editorCameraRepository->rotationSensitivity * 0.25f);
        editorCameraRepository->pitch = glm::clamp(editorCameraRepository->pitch, -MIN_MAX_PITCH, MIN_MAX_PITCH);

        ApplicationEventContext::dispatch("Camera");
    }
}
