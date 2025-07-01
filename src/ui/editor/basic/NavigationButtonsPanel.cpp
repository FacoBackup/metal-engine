#include "NavigationButtonsPanel.h"

#include "../../GuiContext.h"
#include "../../data/Views.h"

namespace Metal {
    void NavigationButtonsPanel::onSync() {
        ImGui::BeginChild(id.c_str());
        for (Views &option: Views::OPTIONS) {
            ImGui::Button(option.icon.c_str());
            ImGui::BeginTooltip();
            ImGui::Text(option.name.c_str());
            ImGui::EndTooltip();
            ImGui::NewLine();
        }
        ImGui::EndChild();
    }
} // Metal
