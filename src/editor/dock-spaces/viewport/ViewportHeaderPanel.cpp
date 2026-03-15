#include "ViewportHeaderPanel.h"
#include "GizmoSettingsPanel.h"
#include "../../../ApplicationContext.h"
#include "../../util/UIUtil.h"
#include "../../repository/EditorRepository.h"
#include "../../../engine/repository/WorldRepository.h"

namespace Metal {
    void ViewportHeaderPanel::onInitialize() {
        gizmo = initializePanel<GizmoSettingsPanel>();
    }

    void ViewportHeaderPanel::onSync() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        ImGui::BeginChild(id.c_str(), ImVec2(0, HEIGHT), false, FLAGS);
        {
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 2, ImGui::GetCursorPosY() + 4));
            gizmo->onSync();
            ImGui::SameLine();
            UIUtil::DynamicSpacing(270);
            ImGui::SameLine();

            cameraMode();
            ImGui::SameLine();

            shadingMode();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }


    void ViewportHeaderPanel::cameraMode() const {
        if (UIUtil::ButtonSimple(Icons::center_focus_strong + id + "centerCamera",
                                 UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            worldRepository->camera.position = {0, 0, 0};
            worldRepository->camera.registerChange();
        }
        UIUtil::RenderTooltip("Center camera?");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(65);
        static const char *speeds[] = {"0.1", "0.5", "1.0", "2.0", "5.0", "10.0"};
        static float speedValues[] = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
        int currentSpeedIndex = 2;
        for (int i = 0; i < 6; i++) {
            if (worldRepository->camera.movementSensitivity == speedValues[i]) {
                currentSpeedIndex = i;
                break;
            }
        }
        if (ImGui::Combo((id + "speedCamera").c_str(), &currentSpeedIndex, speeds, IM_ARRAYSIZE(speeds))) {
            worldRepository->camera.movementSensitivity = speedValues[currentSpeedIndex];
        }
        UIUtil::RenderTooltip("Camera speed");
    }

    void ViewportHeaderPanel::shadingMode() {
        ImGui::SetNextItemWidth(150);
        shadingModelOption = ShadingModes::IndexOfValue(editorRepository->shadingMode);

        auto currentEntry = ShadingModes::entries[shadingModelOption];
        if (ImGui::BeginCombo((id + "shadingMode").c_str(), (currentEntry.icon + " " + currentEntry.label).c_str())) {
            for (int i = 0; i < ShadingModes::entries.size(); i++) {
                const bool is_selected = (shadingModelOption == i);
                auto entry = ShadingModes::entries[i];
                if (ImGui::Selectable((entry.icon + " " + entry.label).c_str(), is_selected)) {
                    shadingModelOption = i;
                    editorRepository->shadingMode = ShadingModes::ValueOfIndex(shadingModelOption);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
} // Metal
