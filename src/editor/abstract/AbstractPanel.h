#ifndef METAL_ENGINE_ABSTRACTPANEL_H
#define METAL_ENGINE_ABSTRACTPANEL_H

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "../../common/IContextMember.h"
#include "../../common/IEventMember.h"
#include "../../common/ISync.h"
#include "../../common/IInit.h"
#include "../../ApplicationContext.h"

namespace Metal {
    class ApplicationContext;

    class AbstractPanel : public IContextMember, public IEventMember, public ISync, public IInit {
    protected:
        std::vector<std::shared_ptr<AbstractPanel>> children;
        const std::string id;

        virtual void onSyncChildren() const;

    public:
        explicit AbstractPanel();

        std::vector<std::shared_ptr<AbstractPanel>> &getChildren();

        template<typename T, typename... Args>
        std::shared_ptr<T> initializePanel(bool addToChildList = true, Args &&... args) {
            auto panel = std::make_shared<T>(std::forward<Args>(args)...);
            panel->ctx = ctx;
            ctx->injectDependencies(panel.get());
            panel->onInitialize();
            if (addToChildList) {
                children.push_back(panel);
            }
            return panel;
        }

        template<typename T>
        std::shared_ptr<T> initializePanel(std::shared_ptr<T> panel, bool addToChildList = true) {
            panel->ctx = ctx;
            ctx->injectDependencies(panel.get());
            panel->onInitialize();
            if (addToChildList) {
                children.push_back(panel);
            }
            return panel;
        }

        void removeAllChildren();

        void removePanel(std::shared_ptr<AbstractPanel> panel);

        virtual void onRemove() {
        }
    };
}
#endif
