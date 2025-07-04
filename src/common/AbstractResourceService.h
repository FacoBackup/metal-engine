#ifndef ABSTRACTRESOURCESERVICE_H
#define ABSTRACTRESOURCESERVICE_H
#include <string>
#include <unordered_map>

#include "AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;
    class RuntimeResource;

    class AbstractResourceService : public AbstractRuntimeComponent {
    protected:
        std::unordered_map<std::string, RuntimeResource *> resources{};

        void registerResource(RuntimeResource *resource);

    public:
        std::unordered_map<std::string, RuntimeResource *> &getResources() {
            return resources;
        }

        void disposeAll();
    };
} // Metal

#endif
