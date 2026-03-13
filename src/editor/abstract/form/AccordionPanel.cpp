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

        ImGui::PushStyleColor(ImGuiCol_Header, applicationContext->themeService.neutralPalette);
        bool open = ImGui::CollapsingHeader(fixedId.c_str(), ImGuiTreeNodeFlags_None);
        ImGui::PopStyleColor();

        if (open) {
            ImGui::Spacing();
            ImGui::Indent(15.0f);
            onSyncChildren();
            ImGui::Unindent(15.0f);
            ImGui::Spacing();
            ImGui::Separator();
        }
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
