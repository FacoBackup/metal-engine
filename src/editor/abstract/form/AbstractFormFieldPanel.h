#ifndef METAL_ENGINE_ABSTRACT_FORM_FIELD_PANEL_H
#define METAL_ENGINE_ABSTRACT_FORM_FIELD_PANEL_H

#include <string>

#include "../AbstractPanel.h"

namespace Metal {
    class ThemeService;

    class HistoryService;

    class AbstractFormFieldPanel : public AbstractPanel {
    protected:
        const std::string* filter = nullptr;

        ThemeService *themeService = nullptr;
        HistoryService *historyService = nullptr;
    public:

        std::vector<Dependency> getDependencies() override {
            return {
                    {"ThemeService", &themeService},
                    {"HistoryService", &historyService}
            };
        }
        [[nodiscard]] virtual bool isVisible() const = 0;

        void onSyncChildren() const override {
            for (const auto &panel: children) {
                if (std::dynamic_pointer_cast<AbstractFormFieldPanel>(panel)->isVisible()) {
                    panel->onSync();
                }
            }
        }
        void setFilter(const std::string* f) {
            filter = f;
        }
    };
}
#endif
