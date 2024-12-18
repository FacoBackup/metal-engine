#include "AbstractResourceService.h"
#include "../../context/ApplicationContext.h"
#include "../../context/runtime/RuntimeResource.h"

namespace Metal {
    void AbstractResourceService::registerResource(RuntimeResource *resource) {
        resources[resource->getId()] = resource;
    }

    void AbstractResourceService::dispose(RuntimeResource *resource) {
        resource->dispose(vulkanContext);
        resources.erase(resource->getId());
        delete resource;
    }

    void AbstractResourceService::disposeAll() {
        for (auto it = resources.begin(); it != resources.end(); ) {
            auto *r = it->second;
            r->dispose(vulkanContext);
            it = resources.erase(it); // Erase and update the iterator
            delete r; // Delete the resource after erasing
        }
    }

    AbstractResourceService::AbstractResourceService(ApplicationContext &context)
        : AbstractRuntimeComponent(context), vulkanContext(context.getVulkanContext()) {
    }
}
