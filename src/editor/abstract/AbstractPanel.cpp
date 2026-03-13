#include "AbstractPanel.h"
#include "../util/Util.h"

namespace Metal {
    void AbstractPanel::appendChild(AbstractPanel *panel) {
        panel->applicationContext = applicationContext;
        panel->onInitialize();
        children.emplace_back(panel);
    }

    void AbstractPanel::onSyncChildren() const {
        for (const auto panel: children) {
            panel->onSync();
        }
    }

    AbstractPanel::AbstractPanel() : id("##" + Util::uuidV4()) {
    }

    void AbstractPanel::removeAllChildren() {
        for (const auto panel: children) {
            panel->removeAllChildren();
            delete panel;
        }
        children.clear();
    }

    std::vector<IPanel *> &AbstractPanel::getChildren() {
        return children;
    }
}
