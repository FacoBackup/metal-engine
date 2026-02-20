#include "ViewportHeaderPanel.h"

#include "GizmoSettingsPanel.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    void ViewportHeaderPanel::onInitialize() {
        appendChild(gizmo = new GizmoSettingsPanel());
    }

    void ViewportHeaderPanel::onSync() {

        gizmo->onSync();
        ImGui::SameLine();
        int option = 0;
        UIUtil::DynamicSpacing(385);
        ImGui::SetNextItemWidth(100);
        if (ImGui::Combo((id + "##entities").c_str(), &option, ComponentTypes::NAMES)) {
            auto id = ApplicationContext::Get().worldRepository.createEntity();
            ApplicationContext::Get().worldRepository.createComponent(id, ComponentTypes::ValueOfIndex(option));
            ApplicationContext::Get().selectionService.clearSelection();
            ApplicationContext::Get().selectionService.addSelected(id);
        }
        ImGui::SameLine();

        cameraMode();
        ImGui::SameLine();

        shadingMode();
    }


    void ViewportHeaderPanel::cameraMode() const {
        if (UIUtil::ButtonSimple(Icons::center_focus_strong + id + "centerCamera",
                                 UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            ApplicationContext::Get().worldRepository.camera.position = {0, 0, 0};
            ApplicationContext::Get().worldRepository.camera.registerChange();
        }
        UIUtil::RenderTooltip("Center camera?");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(75);
        ImGui::DragFloat((id + "speedCamera").c_str(),
                         &ApplicationContext::Get().worldRepository.camera.movementSensitivity, .1f,
                         .1f);
        UIUtil::RenderTooltip("Camera speed");

    }

    void ViewportHeaderPanel::shadingMode() {
        auto &editorRepository = ApplicationContext::Get().editorRepository;
        ImGui::SetNextItemWidth(150);
        shadingModelOption = IndexOfValue(editorRepository.shadingMode);
        if (ImGui::Combo((id + "shadingMode").c_str(), &shadingModelOption, ShadingMode::Names)) {
            editorRepository.shadingMode = ShadingMode::ValueOfIndex(shadingModelOption);
        }
    }
} // Metal
