#ifndef METAL_ENGINE_ABSTRACT_FORM_FIELD_PANEL_H
#define METAL_ENGINE_ABSTRACT_FORM_FIELD_PANEL_H

#include <string>

#include "../AbstractPanel.h"

namespace Metal {
    class AbstractFormFieldPanel : public AbstractPanel {
    protected:
        const std::string* filter = nullptr;
    public:
        [[nodiscard]] virtual bool isVisible() const = 0;

        void onSyncChildren() const override {
            for (const auto panel: children) {
                if (dynamic_cast<AbstractFormFieldPanel *>(panel)->isVisible()) {
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
