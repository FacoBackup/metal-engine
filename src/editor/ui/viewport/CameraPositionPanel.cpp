#include "CameraPositionPanel.h"

#include "../../../context/ApplicationContext.h"
#include "../../common/UIUtil.h"

namespace Metal {
    void CameraPositionPanel::onSync() {
        UIUtil::AUX_VEC2.x = ImGui::GetWindowPos().x;
        UIUtil::AUX_VEC2.y = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - 25;
        ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);

        UIUtil::AUX_VEC2.x = ImGui::GetWindowSize().x;
        UIUtil::AUX_VEC2.y = 16;
        ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, UIUtil::DEFAULT_PADDING);
        ImGui::SetNextWindowBgAlpha(.4f);
        if (ImGui::Begin(id.c_str(), &UIUtil::OPEN, UIUtil::FIXED_WINDOW_FLAGS)) {
            auto &positionCamera = context->getEngineContext().cameraRepository.currentCamera->position;
            // ImGui::Text(
            // "Current tile: " + worldService.getCurrentTile().getX() + " " + worldService.getCurrentTile().getZ());
            // ImGui::SameLine();
            ImGui::TextColored(RED, "X: %i", static_cast<int>(positionCamera.x));
            ImGui::SameLine();
            ImGui::TextColored(GREEN, "Y: %i", static_cast<int>(positionCamera.y));
            ImGui::SameLine();
            ImGui::TextColored(BLUE, "Z: %i", static_cast<int>(positionCamera.z));
            ImGui::SameLine();

            ImGui::Text(
                "Yaw: %i", static_cast<int>(context->getEngineContext().cameraRepository.currentCamera->yaw * TO_DEG));
            ImGui::SameLine();
            ImGui::Text("Pitch: %i",
                        static_cast<int>(context->getEngineContext().cameraRepository.currentCamera->pitch * TO_DEG));
        }
        ImGui::PopStyleVar();
        ImGui::End();
    }
} // Metal