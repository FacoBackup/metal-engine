#include "GlobalSettingsPanel.h"

#include "../../../context/ApplicationContext.h"
#include "../../../engine/enum/EditorMode.h"
#include "../../../common/util/UIUtil.h"
#include "../../../engine/service/camera/Camera.h"

namespace Metal {

    void GlobalSettingsPanel::onSync() {
        auto &editorRepository = context->getEditorContext().editorRepository;

        ImGui::SetNextItemWidth(150);
        editorMode = EditorMode::IndexOfValue(editorRepository.editorMode);
        if (ImGui::Combo(id.c_str(), &editorMode, EditorMode::Names)) {
            editorRepository.editorMode = EditorMode::ValueOfIndex(editorMode);
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        int option = 0;

        if (ImGui::Combo((id + "##entities").c_str(), &option, options)) {
            auto id = context->getEngineContext().worldRepository.createEntity();
            context->getEngineContext().worldRepository.createComponent(id, ComponentTypes::ValueOfIndex(option - 1));
            context->getEditorContext().selectionService.clearSelection();
            context->getEditorContext().selectionService.addSelected(id);
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
            context->getEngineContext().worldRepository.camera.position = {0, 0, 0};
            context->getEngineContext().worldRepository.camera.registerChange();
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(75);
        ImGui::DragFloat("##speedCamera", &context->getEngineContext().worldRepository.camera.movementSensitivity, .1f,
                         .1f);
    }

    void GlobalSettingsPanel::shadingMode() {
        auto &editorRepository = context->getEditorContext().editorRepository;

        UIUtil::DynamicSpacing(405);

        ImGui::Text("Shading");

        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::grid_on + "##world", editorRepository.gridOverlayObjects, true,
                                 editorRepository.accent)) {
            editorRepository.gridOverlayObjects = !editorRepository.gridOverlayObjects;
        }

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
