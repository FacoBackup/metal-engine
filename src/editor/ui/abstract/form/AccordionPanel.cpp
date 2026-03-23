#include "AccordionPanel.h"
#include "imgui.h"
#include "ApplicationContext.h"
#include "editor/service/ThemeService.h"
#include "editor/ui/UIUtil.h"

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

        ImVec4 color = backgroundColor.w == 0 ? themeService->palette3 : backgroundColor;
        if (UIUtil::Accordion(fixedId, title, isOpen, color)) {
            ImGui::Indent(15.0f);
            onSyncChildren();
            ImGui::Unindent(15.0f);
            UIUtil::EndAccordion();
        }
    }

    bool AccordionPanel::isVisible() const {
        if (!filter || filter->empty()) return true;

        for (const auto &panel : children) {
            if (auto abstractPanel = std::dynamic_pointer_cast<AbstractFormFieldPanel>(panel)) {
                if (abstractPanel->isVisible()) return true;
            }
        }
        return false;
    }
}
