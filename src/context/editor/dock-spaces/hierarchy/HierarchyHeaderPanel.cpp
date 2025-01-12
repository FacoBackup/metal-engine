#include "HierarchyHeaderPanel.h"

#include "../../../../context/ApplicationContext.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    void HierarchyHeaderPanel::onSync() {
        ImGui::InputText(("##hierarchySearch" + id).c_str(), search, sizeof(search));
        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::inventory_2 + "##hierarchyAdd" + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            context->worldRepository.createEntity();
        }
        UIUtil::RenderTooltip("Create folder");
    }
} // Metal
