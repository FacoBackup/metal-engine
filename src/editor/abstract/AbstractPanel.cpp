#include "AbstractPanel.h"
#include "../util/Util.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void AbstractPanel::onSyncChildren() const {
        for (const auto &panel: children) {
            panel->onSync();
        }
    }

    AbstractPanel::AbstractPanel() : id("##" + Util::uuidV4()) {
    }

    void AbstractPanel::removeAllChildren() {
        for (const auto &panel: children) {
            panel->removeAllChildren();
        }
        children.clear();
    }

    std::vector<std::shared_ptr<AbstractPanel>> &AbstractPanel::getChildren() {
        return children;
    }

    void AbstractPanel::removePanel(std::shared_ptr<AbstractPanel> panel) {
        auto it = std::find(children.begin(), children.end(), panel);
        if (it != children.end()) {
            children.erase(it);
        }
    }
}
