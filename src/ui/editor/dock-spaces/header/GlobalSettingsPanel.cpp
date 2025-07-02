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

        shadingMode();
    }


    void GlobalSettingsPanel::shadingMode() {
        auto &editorRepository = SINGLETONS.editorRepository;


        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        shadingModelOption = IndexOfValue(editorRepository.shadingMode);
        if (ImGui::Combo("##shadingMode", &shadingModelOption, ShadingMode::Names)) {
            editorRepository.shadingMode = ShadingMode::ValueOfIndex(shadingModelOption);
        }
    }
}
