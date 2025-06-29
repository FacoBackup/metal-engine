#include "GlobalSettingsPanel.h"

#include "../../../../MetalContextProvider.h"
#include "../../../util/UIUtil.h"
#include "../../../../runtime/data/Camera.h"

namespace Metal {
    void GlobalSettingsPanel::onSync() {
        if (ImGui::Button((Icons::apps + "Build BVH" + id + "bvh").c_str())) {
            SINGLETONS.engineContext.dispatchBVHBuild();
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(150);
        int option = 0;

        if (ImGui::Combo((id + "##entities").c_str(), &option, ComponentTypes::NAMES)) {
            auto id = SINGLETONS.worldRepository.createEntity();
            SINGLETONS.worldRepository.createComponent(id, ComponentTypes::ValueOfIndex(option));
            SINGLETONS.selectionService.clearSelection();
            SINGLETONS.selectionService.addSelected(id);
        }

        cameraMode();
        shadingMode();
    }


    void GlobalSettingsPanel::cameraMode() const {
        UIUtil::DynamicSpacing(575);
        ImGui::Text("Camera");
        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::center_focus_strong + "##centerCamera", UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            SINGLETONS.worldRepository.camera.position = {0, 0, 0};
            SINGLETONS.worldRepository.camera.registerChange();
        }
        UIUtil::RenderTooltip("Center camera?");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(75);
        ImGui::DragFloat("##speedCamera", &SINGLETONS.worldRepository.camera.movementSensitivity, .1f,
                         .1f);
    }

    void GlobalSettingsPanel::shadingMode() {
        auto &editorRepository = SINGLETONS.editorRepository;

        UIUtil::DynamicSpacing(340);
        ImGui::Text("Shading");

        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::circle + "Lit##litShading",
                                 editorRepository.shadingMode == ShadingMode::ShadingMode::LIT, false,
                                 editorRepository.accent)) {
            editorRepository.shadingMode = ShadingMode::ShadingMode::LIT;
        }

        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::palette + "Random##randomShading",
                                 editorRepository.shadingMode == ShadingMode::ShadingMode::RANDOM, false,
                                 editorRepository.accent)) {
            editorRepository.shadingMode = ShadingMode::ShadingMode::RANDOM;
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        shadingModelOption = IndexOfValue(editorRepository.shadingMode);
        if (ImGui::Combo("##shadingMode", &shadingModelOption, ShadingMode::Names)) {
            editorRepository.shadingMode = ShadingMode::ValueOfIndex(shadingModelOption);
        }
    }
}
