#include "AccordionPanel.h"
#include "imgui.h"
#include "../../../ApplicationContext.h"

namespace Metal {
    void AccordionPanel::setTitle(const std::string &t) {
        this->title = t;
        this->fixedId = t + id;
    }

    void AccordionPanel::onSync() {
        if (title.empty()) {
            onSyncChildren();
            return;
        }
        ImGui::PushStyleColor(ImGuiCol_Header, CTX.themeService.neutralPalette);
        if (ImGui::CollapsingHeader(fixedId.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            onSyncChildren();
        }
        ImGui::PopStyleColor();
    }

    bool AccordionPanel::isVisible() const {
        if (!filter || filter->empty()) return true;

        for (const auto panel : children) {
            if (auto abstractPanel = dynamic_cast<AbstractFormFieldPanel*>(panel)) {
                if (abstractPanel->isVisible()) return true;
            }
        }
        return false;
    }
}
