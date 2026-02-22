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
        if (ImGui::CollapsingHeader(fixedId.c_str())) {
            onSyncChildren();
        }
        ImGui::PopStyleColor();
    }

    void AccordionPanel::onSyncChildren() const {
        for (int i = 0; i < children.size(); i++) {
            children[i]->onSync();

            if (i < children.size() - 1) {
                ImGui::Dummy(ImVec2(0, 4));
                ImGui::Separator();
            }
        }
    }
}
