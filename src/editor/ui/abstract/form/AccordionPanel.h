#ifndef ACCORDION_H
#define ACCORDION_H

#include "AbstractFormFieldPanel.h"
#include <imgui.h>

namespace Metal {
    class ThemeService;

    class AccordionPanel final : public AbstractFormFieldPanel {
        std::string fixedId = id;
        std::string title;
        bool isOpen = false;
        ImVec4 backgroundColor = {0, 0, 0, 0};

    public:
        void setTitle(const std::string &title);

        void setBackgroundColor(const ImVec4 &color) { this->backgroundColor = color; }

        void onSync() override;

        bool isVisible() const override;
    };
}

#endif
