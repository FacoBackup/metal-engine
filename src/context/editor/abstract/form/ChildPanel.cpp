#include "ChildPanel.h"
#include "imgui.h"
#include "../../../ApplicationContext.h"

namespace Metal {
    void ChildPanel::setTitle(const std::string &t) {
        this->title = t;
        this->fixedId = t + id;
    }

    void ChildPanel::onSync() {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, CTX.themeService.palette0);
        if (!title.empty()) {
            ImGui::Spacing();
            ImGui::Text(title.c_str());
            ImGui::Separator();
        }
        onSyncChildren();

        ImGui::PopStyleColor();
    }

    bool ChildPanel::isVisible() const {
        if (!filter || filter->empty()) return true;

        for (const auto panel: children) {
            if (auto abstractPanel = dynamic_cast<AbstractFormFieldPanel *>(panel)) {
                if (abstractPanel->isVisible()) return true;
            }
        }
        return false;
    }
}
