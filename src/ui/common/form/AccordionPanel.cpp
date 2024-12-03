#include "AccordionPanel.h"

namespace Metal {
    void AccordionPanel::setTitle(const std::string &title) {
        this->title = title;
        this->fixedId = (title + id).c_str();
    }

    void AccordionPanel::onSync() {
        if (title.empty()) {
            renderChildren();
            return;
        }
        if (ImGui::CollapsingHeader(fixedId)) {
            renderChildren();
        }
    }
}
