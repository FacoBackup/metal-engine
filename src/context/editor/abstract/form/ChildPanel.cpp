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
        if (ImGui::BeginChild(fixedId.c_str(),  ImVec2(0, 0), ImGuiChildFlags_Border)) {
            if (!title.empty()) {
                ImGui::Text(title.c_str());
                ImGui::Separator();
            }
            onSyncChildren();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
    }

    void ChildPanel::onSyncChildren() const {
        for (int i = 0; i < children.size(); i++) {
            children[i]->onSync();

            if (i < children.size() - 1) {
                ImGui::Dummy(ImVec2(0, 4));
                ImGui::Separator();
            }
        }
    }
}
