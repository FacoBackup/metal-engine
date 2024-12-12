#ifndef ABSTRACTRESOURCESERVICE_H
#define ABSTRACTRESOURCESERVICE_H
#include "AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;
    class RuntimeResource;
    struct ResourceRepository;

    class ApplicationContext;

    class AbstractResourceService : public AbstractRuntimeComponent {
    protected:
        VulkanContext &vulkanContext;
        ResourceRepository &repository;

        void registerResource(RuntimeResource *resource) const;

    public:
        explicit AbstractResourceService(ApplicationContext &context);
    };
} // Metal

#endif
