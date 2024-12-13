#include "ViewportPanel.h"

#include "CameraPositionPanel.h"
#include "ImGuizmo.h"
#include "../../../context/ApplicationContext.h"
#include "../../../context/runtime/DescriptorInstance.h"
#include "../../../context/runtime/FrameBufferInstance.h"
#include "../../../context/runtime/FrameBufferAttachment.h"

namespace Metal {
    void ViewportPanel::onInitialize() {
        appendChild(new CameraPositionPanel);
    }

    void ViewportPanel::onSync() {
        auto &coreSets = context->getVulkanContext().coreDescriptorSets;
        coreSets.updateImageSamplerDescriptor(
            context->getVulkanContext().coreFrameBuffers.auxRenderPass->
            attachments[0]->vkImageSampler,
            context->getVulkanContext().coreFrameBuffers.auxRenderPass->
            attachments[0]->vkImageView
        );

        updateInputs();
        updateCamera();
        ImGui::Image(
            reinterpret_cast<ImTextureID>(coreSets.imageSampler->vkDescriptorSet),
            ImVec2{size->x, size->y});
        onSyncChildren();
    }

    void ViewportPanel::updateCamera() {
        auto &cameraRepository = context->getEngineContext().cameraRepository;
        auto &cameraMovementService = context->getEngineContext().cameraMovementService;
        auto &editorRepository = context->getEditorContext().editorRepository;

        auto *camera = editorRepository.viewportCamera[dock->id];
        if (camera == nullptr) {
            camera = new Camera{};
            editorRepository.viewportCamera[dock->id] = camera;
            camera->pitch = -(glm::pi<float>() / 4);
            camera->yaw = glm::pi<float>() / 4;
            camera->position.x = 10;
            camera->position.y = 10;
            camera->position.z = 10;
        }
        cameraRepository.currentCamera = camera;

        if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            cameraMovementService.handleInput(*camera, isFirstMovement);
            if (const auto &io = ImGui::GetIO(); io.MouseWheel != 0) {
                cameraRepository.movementSensitivity += io.MouseWheel * 100 * context->getEngineContext().deltaTime;
                cameraRepository.movementSensitivity = std::max(.1f, cameraRepository.movementSensitivity);
            }
            isFirstMovement = false;
        } else {
            isFirstMovement = true;
        }
    }

    void ViewportPanel::updateInputs() const {
        auto &repo = context->getEngineContext().runtimeRepository;
        const ImVec2 windowSize = ImGui::GetWindowSize();
        size->x = windowSize.x;
        size->y = windowSize.y - DockSpacePanel::FRAME_SIZE;

        repo.viewportH = size->y;
        repo.viewportW = size->x;

        const ImVec2 windowPos = ImGui::GetWindowPos();
        repo.viewportX = windowPos.x;
        repo.viewportY = windowPos.y + DockSpacePanel::FRAME_SIZE;

        repo.isFocused = ImGui::IsWindowHovered();
        repo.fasterPressed = ImGui::IsKeyDown(ImGuiKey_LeftShift);
        repo.forwardPressed = ImGui::IsKeyDown(ImGuiKey_W);
        repo.backwardPressed = ImGui::IsKeyDown(ImGuiKey_S);
        repo.leftPressed = ImGui::IsKeyDown(ImGuiKey_A);
        repo.rightPressed = ImGui::IsKeyDown(ImGuiKey_D);
        repo.upPressed = ImGui::IsKeyDown(ImGuiKey_Space);
        repo.downPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
        repo.mousePressed = ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseDown(
                                ImGuiMouseButton_Left);

        const ImVec2 mousePos = ImGui::GetMousePos();
        repo.mouseX = mousePos.x;
        repo.mouseY = mousePos.y;

        repo.normalizedMouseX = (repo.mouseX + repo.viewportX) / repo.viewportW;
        repo.normalizedMouseY = (repo.viewportH - repo.mouseY + repo.viewportY) / repo.viewportH;
    }
}
