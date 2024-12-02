#include "AbstractPanel.h"

namespace Metal {
    void AbstractPanel::appendChild(AbstractPanel *panel) {
        panel->setContext(context);
        panel->onInitialize();
        children.push_back(panel);
    }

    void AbstractPanel::renderChildren() const {
        for (const auto panel: children) {
            panel->onSync();
        }
    }

    void AbstractPanel::removeAllChildren() {
        for (const auto panel: children) {
            panel->removeAllChildren();
            delete panel;
        }
        children.clear();
    }

    void AbstractPanel::setContext(ApplicationContext *context) {
        if (context == nullptr) {
            this->context = context;
        }
    }
}
