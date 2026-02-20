#include "ViewportHeaderPanel.h"

#include "GizmoSettingsPanel.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    void ViewportHeaderPanel::onInitialize() {
        appendChild(gizmo = new GizmoSettingsPanel());
    }

    void ViewportHeaderPanel::onSync() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        if (ImGui::BeginChild(id.c_str(), ImVec2(0, HEIGHT), false, FLAGS)) {
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 2, ImGui::GetCursorPosY() + 4));
            gizmo->onSync();
            ImGui::SameLine();
            int option = 0;
            UIUtil::DynamicSpacing(365);
            ImGui::SetNextItemWidth(100);
            if (ImGui::Combo((id + "##entities").c_str(), &option, ComponentTypes::NAMES)) {
                auto id = CTX.worldRepository.createEntity();
                CTX.worldRepository.createComponent(id, ComponentTypes::ValueOfIndex(option));
                CTX.selectionService.clearSelection();
                CTX.selectionService.addSelected(id);
            }
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
            CTX.worldRepository.camera.position = {0, 0, 0};
            CTX.worldRepository.camera.registerChange();
        }
        UIUtil::RenderTooltip("Center camera?");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(65);
        static const char *speeds[] = {"0.1", "0.5", "1.0", "2.0", "5.0", "10.0"};
        static float speedValues[] = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
        int currentSpeedIndex = 2;
        for (int i = 0; i < 6; i++) {
            if (CTX.worldRepository.camera.movementSensitivity == speedValues[i]) {
                currentSpeedIndex = i;
                break;
            }
        }
        if (ImGui::Combo((id + "speedCamera").c_str(), &currentSpeedIndex, speeds, IM_ARRAYSIZE(speeds))) {
            CTX.worldRepository.camera.movementSensitivity = speedValues[currentSpeedIndex];
        }
        UIUtil::RenderTooltip("Camera speed");
    }

    void ViewportHeaderPanel::shadingMode() {
        auto &editorRepository = CTX.editorRepository;
        ImGui::SetNextItemWidth(150);
        shadingModelOption = IndexOfValue(editorRepository.shadingMode);

        static const std::string names[] = {
            Icons::lightbulb + " Lit",
            Icons::image + " Albedo",
            Icons::gradient + " Normal",
            Icons::texture + " Roughness",
            Icons::blur_on + " Metallic",
            Icons::contrast + " Occlusion",
            Icons::casino + " Random",
            Icons::layers + " Depth",
            Icons::grid_on + " UV",
            Icons::place + " Position",
            Icons::highlight + " Lighting only",
            Icons::wb_incandescent + " Emission",
            Icons::ipublic + " Global Illumination"
        };

        if (ImGui::BeginCombo((id + "shadingMode").c_str(), names[shadingModelOption].c_str())) {
            for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                const bool is_selected = (shadingModelOption == i);
                if (ImGui::Selectable(names[i].c_str(), is_selected)) {
                    shadingModelOption = i;
                    editorRepository.shadingMode = ShadingMode::ValueOfIndex(shadingModelOption);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
} // Metal
