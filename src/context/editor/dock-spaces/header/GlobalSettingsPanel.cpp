#include "GlobalSettingsPanel.h"

#include "../../../../context/ApplicationContext.h"
#include "../../../../enum/EditorMode.h"
#include "../../../../util/UIUtil.h"
#include "../../../../service/camera/Camera.h"

namespace Metal {
    void GlobalSettingsPanel::onSync() {
        auto &editorRepository = context->editorRepository;

        if (ImGui::Button((Icons::apps + "Build BVH" + id + "voxelize").c_str())) {
            context->engineContext.dispatchBVHBuild();
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(150);
        editorMode = EditorMode::IndexOfValue(editorRepository.editorMode);
        if (ImGui::Combo(id.c_str(), &editorMode, EditorMode::Names)) {
            editorRepository.editorMode = EditorMode::ValueOfIndex(editorMode);
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        int option = 0;

        if (ImGui::Combo((id + "##entities").c_str(), &option, ComponentTypes::NAMES)) {
            auto id = context->worldRepository.createEntity();
            context->worldRepository.createComponent(id, ComponentTypes::ValueOfIndex(option));
            context->selectionService.clearSelection();
            context->selectionService.addSelected(id);
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
            context->worldRepository.camera.position = {0, 0, 0};
            context->worldRepository.camera.registerChange();
        }
        UIUtil::RenderTooltip("Center camera?");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(75);
        ImGui::DragFloat("##speedCamera", &context->worldRepository.camera.movementSensitivity, .1f,
                         .1f);
    }

    void GlobalSettingsPanel::shadingMode() {
        auto &editorRepository = context->editorRepository;

        UIUtil::DynamicSpacing(405);

        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::grid_on + "##voxels", editorRepository.showVoxels, true,
                                 editorRepository.accent)) {
            editorRepository.showVoxels = !editorRepository.showVoxels;
        }

        UIUtil::RenderTooltip("Show voxels?");
        UIUtil::Spacing();
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
