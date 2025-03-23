#include "AbstractPanel.h"
#include "../../../util/Util.h"

namespace Metal {
    void AbstractPanel::appendChild(AbstractPanel *panel) {
        panel->setContext(context);
        panel->onInitialize();
        children.push_back(panel);
    }

    void AbstractPanel::onSyncChildren() const {
        for (const auto panel: children) {
            panel->onSync();
        }
    }

    AbstractPanel::AbstractPanel(): id("##" + Util::uuidV4()) {
    }

    void AbstractPanel::removeAllChildren() {
        for (const auto panel: children) {
            panel->removeAllChildren();
            delete panel;
        }
        children.clear();
    }

    void AbstractPanel::setContext(ApplicationContext *context) {
        if (this->context == nullptr) {
            this->context = context;
        }
    }

    void AbstractPanel::removeChild(AbstractPanel *view) {
        int index = -1;
        for (int i = 0; i < children.size(); i++) {
            if (children[i] == view) {
                index = i;
            }
        }
        children[index]->removeAllChildren();
        delete children[index];
        children.erase(children.begin() + index);
    }

    std::vector<IPanel *> &AbstractPanel::getChildren() {
        return children;
    }
}
