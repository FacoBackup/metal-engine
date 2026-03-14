#include "AbstractPanel.h"
#include "../util/Util.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void AbstractPanel::appendChild(AbstractPanel *panel) {
        ctx->injectDependencies(panel);
        panel->onInitialize();
        children.push_back(panel);
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

    std::vector<AbstractPanel *> &AbstractPanel::getChildren() {
        return children;
    }
}
