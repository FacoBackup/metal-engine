
#include "AbstractPanel.h"

namespace Metal {

    void Metal::AbstractPanel::appendChild(AbstractPanel *panel) {
        panel->setContext(context);
        panel->onInitialize();
        panels.push_back(panel);
    }

    void AbstractPanel::setContext(ApplicationContext *context) {
        if (context == nullptr) {
            this->context = context;
        }
    }
}