#ifndef ABSTRACTRESOURCESERVICE_H
#define ABSTRACTRESOURCESERVICE_H
#include <mutex>
#include <string>
#include <unordered_map>

#include "AbstractRuntimeComponent.h"
#include "../context/engine/resource/RuntimeResource.h"

namespace Metal {
    class VulkanContext;

    template<typename T>
    class AbstractResourceService : public AbstractRuntimeComponent {
        static_assert(std::is_base_of_v<RuntimeResource, T>, "T must be a subclass of RuntimeResource");

    protected:
        std::unordered_map<std::string, T *> resources{};
        mutable std::mutex resourceMutex;

        template<typename... Args>
        T *createResourceInstance(const std::string &id, Args &&... args) {
            std::lock_guard lock(resourceMutex);
            auto *resource = new T(id, std::forward<Args>(args)...);
            resources[id] = resource;
            return resource;
        }

    public:
        T *getResource(const std::string &id) const {
            std::lock_guard lock(resourceMutex);
            if (resources.contains(id)) {
                return resources.at(id);
            }
            return nullptr;
        }

        std::unordered_map<std::string, T *> &getResources() {
            std::lock_guard lock(resourceMutex);
            return resources;
        }

        virtual void disposeResource(T *resource) = 0;

        virtual void dispose(std::string id) {
            std::lock_guard lock(resourceMutex);
            if (resources.contains(id)) {
                auto *r = resources.at(id);
                disposeResource(r);
                resources.erase(id);
                delete r;
            }
        }

        void disposeAll() {
            std::lock_guard lock(resourceMutex);
            for (auto it = resources.begin(); it != resources.end();) {
                auto *r = it->second;
                disposeResource(r);
                delete r;
                it = resources.erase(it);
            }
        }
    };
} // Metal

#endif
