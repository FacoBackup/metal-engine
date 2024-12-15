#include "AccordionPanel.h"
#include "imgui.h"

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
        if (ImGui::CollapsingHeader(fixedId.c_str())) {
            onSyncChildren();
        }
    }
}
