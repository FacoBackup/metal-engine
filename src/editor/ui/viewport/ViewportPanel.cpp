#include "ViewportPanel.h"

#include "CameraPositionPanel.h"
#include "GizmoPanel.h"
#include "ImGuizmo.h"
#include "ViewportHeaderPanel.h"
#include "../../../context/ApplicationContext.h"
#include "../../../context/runtime/DescriptorInstance.h"
#include "../../../context/runtime/FrameBufferInstance.h"
#include "../../../context/runtime/FrameBufferAttachment.h"
#include "../../../engine/service/camera/Camera.h"

namespace Metal {
    void ViewportPanel::onInitialize() {
        appendChild(headerPanel = new ViewportHeaderPanel());
        appendChild(gizmoPanel = new GizmoPanel(position, size));
        appendChild(cameraPanel = new CameraPositionPanel());
    }

    void ViewportPanel::onSync() {
        updateCamera();
        updateInputs();

        context->guiContext.renderImage(context->getVulkanContext().coreFrameBuffers.auxRenderPass->attachments[0].get(), size->x, size->y);
        if (context->getEditorContext().editorRepository.editorMode == EditorMode::EditorMode::TRANSFORM) {
            gizmoPanel->onSync();
        }
        headerPanel->onSync();
        cameraPanel->onSync();
    }

    void ViewportPanel::updateCamera() {
        auto &worldRepository = context->getEngineContext().worldRepository;
        const auto &cameraService = context->getEngineContext().cameraService;

        if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            cameraService.handleInput(isFirstMovement);
            if (const auto &io = ImGui::GetIO(); io.MouseWheel != 0) {
                worldRepository.camera.movementSensitivity += io.MouseWheel * 100 * context->getEngineContext().
                        deltaTime;
                worldRepository.camera.movementSensitivity =
                        std::max(.1f, worldRepository.camera.movementSensitivity);
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
        size->y = windowSize.y;

        repo.viewportH = size->y;
        repo.viewportW = size->x;

        const ImVec2 windowPos = ImGui::GetWindowPos();
        repo.viewportX = windowPos.x;
        repo.viewportY = windowPos.y;

        repo.isFocused = ImGui::IsWindowHovered();
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
