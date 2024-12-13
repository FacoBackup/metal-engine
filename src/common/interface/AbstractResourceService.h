#ifndef ABSTRACTRESOURCESERVICE_H
#define ABSTRACTRESOURCESERVICE_H
#include <string>
#include <unordered_map>

#include "AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;
    class RuntimeResource;

    class AbstractResourceService : public AbstractRuntimeComponent {
        std::unordered_map<std::string, RuntimeResource *> resources{};

    protected:
        VulkanContext &vulkanContext;

        void registerResource(RuntimeResource *resource);

        void dispose(RuntimeResource *resource);

    public:
        void disposeAll();

        explicit AbstractResourceService(ApplicationContext &context);
    };
} // Metal

#endif
