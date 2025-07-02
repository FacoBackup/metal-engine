#include "NavigationButtonsPanel.h"

#include "../../GuiContext.h"
#include "../../../MetalContextProvider.h"
#include "../../data/Views.h"
#include "../../util/UIUtil.h"

namespace Metal {
    void NavigationButtonsPanel::onSync() {
        ImGui::BeginChild(id.c_str());
        for (Views &option: Views::OPTIONS) {
            if (ImGui::Button((option.icon + id).c_str())) {
                SINGLETONS.editorRepository.viewsSelected.insert_or_assign(position, Views::FindIndexByName(option.name));
            }
            UIUtil::RenderTooltip(option.name);
            ImGui::NewLine();
        }
        ImGui::EndChild();
    }
} // Metal
