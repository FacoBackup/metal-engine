#include "CameraPositionPanel.h"

#include "../../../../MetalContextProvider.h"
#include "../../../util/UIUtil.h"
#include "../../../../runtime/data/Camera.h"

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
        if (ImGui::Begin(id.c_str(), &UIUtil::OPEN, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
            const auto &positionCamera = SINGLETONS.worldRepository.camera.position;
            ImGui::TextColored(RED, "X: %i", static_cast<int>(positionCamera.x));
            ImGui::SameLine();
            ImGui::TextColored(GREEN, "Y: %i", static_cast<int>(positionCamera.y));
            ImGui::SameLine();
            ImGui::TextColored(BLUE, "Z: %i", static_cast<int>(positionCamera.z));
            ImGui::SameLine();

            ImGui::Text(
                "Yaw: %i", static_cast<int>(SINGLETONS.worldRepository.camera.yaw * TO_DEG));
            ImGui::SameLine();
            ImGui::Text("Pitch: %i",
                        static_cast<int>(SINGLETONS.worldRepository.camera.pitch * TO_DEG));
        }
        ImGui::PopStyleVar();
        ImGui::End();
    }
} // Metal
