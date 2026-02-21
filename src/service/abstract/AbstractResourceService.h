#ifndef ABSTRACTRESOURCESERVICE_H
#define ABSTRACTRESOURCESERVICE_H
#include <mutex>
#include <string>
#include <unordered_map>

#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;
    class RuntimeResource;

    class AbstractResourceService : public AbstractRuntimeComponent {
    protected:
        std::unordered_map<std::string, RuntimeResource *> resources{};
        mutable std::mutex resourceMutex;

        void registerResource(RuntimeResource *resource);

    public:
        std::unordered_map<std::string, RuntimeResource *> &getResources() {
            std::lock_guard<std::mutex> lock(resourceMutex);
            return resources;
        }

        void disposeAll();
    };
} // Metal

#endif
