#include "AbstractPanel.h"
#include "../util/Util.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void AbstractPanel::initializePanel(AbstractPanel *panel, bool addToChildList) {
        panel->ctx = ctx;
        ctx->injectDependencies(panel);
        panel->onInitialize();
        if (addToChildList) {
            children.push_back(panel);
        }
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
