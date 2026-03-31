#ifndef METAL_ENGINE_ABSTRACTPANEL_H
#define METAL_ENGINE_ABSTRACTPANEL_H

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <functional>

#include "common/IEventMember.h"
#include "common/ISync.h"
#include "common/IInit.h"
#include "common/LoggerUtil.h"

namespace Metal {
    struct Dependency {
        std::string name;
        void *ptr;
    };

    class AbstractPanel : public IEventMember, public ISync, public IInit {
    public:
        using ServiceProvider = std::function<void *(const std::string &)>;
        static inline ServiceProvider serviceProvider;

    protected:
        std::vector<std::shared_ptr<AbstractPanel>> children;
        const std::string id;

        virtual void onSyncChildren() const;

    public:
        void performInjection() {
            if (!serviceProvider) {
                LOG_WARN("ServiceProvider not set in AbstractPanel. Injection skipped for " + id);
                return;
            }

            for (auto &dep: getDependencies()) {
                void *service = serviceProvider(dep.name);
                if (service && dep.ptr) {
                    *static_cast<void **>(dep.ptr) = service;
                } else if (!service) {
                    LOG_WARN("Could not resolve dependency " + dep.name + " for " + id);
                }
            }
        }

    public:
        explicit AbstractPanel();

        virtual std::vector<Dependency> getDependencies() {
            return {};
        }

        std::vector<std::shared_ptr<AbstractPanel>> &getChildren();

        template<typename T, typename... Args>
        std::shared_ptr<T> initializePanel(bool addToChildList = true, Args &&... args) {
            auto panel = std::make_shared<T>(std::forward<Args>(args)...);
            panel->performInjection();
            panel->onInitialize();
            if (addToChildList) {
                children.push_back(panel);
            }
            return panel;
        }

        template<typename T>
        std::shared_ptr<T> initializePanel(std::shared_ptr<T> panel, bool addToChildList = true) {
            panel->performInjection();
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
