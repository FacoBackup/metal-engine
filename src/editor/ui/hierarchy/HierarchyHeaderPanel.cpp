#include "HierarchyHeaderPanel.h"

#include "../../../context/ApplicationContext.h"
#include "../../../common/util/UIUtil.h"

namespace Metal {
    HierarchyHeaderPanel::HierarchyHeaderPanel(char *&search): search(search) {
    }

    void HierarchyHeaderPanel::onSync() {
        auto &editorRepository = context->getEditorContext().editorRepository;
        ImGui::InputText("##hierarchySearch", search, 128);
        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::inventory_2 + "##hierarchyAdd", UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            context->getEngineContext().worldRepository.createEntity();
        }
        ImGui::SameLine();
        bool show = editorRepository.showOnlyEntitiesHierarchy;
        if (UIUtil::ButtonSimple(
            show ? Icons::filter_list_off + "##hierarchyFilter" : Icons::filter_list + "##hierarchyFilter",
            UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            editorRepository.showOnlyEntitiesHierarchy = !show;
        }
    }
} // Metal
