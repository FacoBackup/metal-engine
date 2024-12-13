#include "AbstractResourceService.h"
#include "../../context/ApplicationContext.h"
#include "../../context/runtime/RuntimeResource.h"

namespace Metal {
    void AbstractResourceService::registerResource(RuntimeResource *resource) {
        resources[resource->id] = resource;
    }

    void AbstractResourceService::dispose(RuntimeResource *resource) {
        resource->dispose(vulkanContext);
        resources.erase(resource->id);
        delete resource;
    }

    void AbstractResourceService::disposeAll() {
        for (auto &entry: resources) {
            dispose(entry.second);
        }
    }

    AbstractResourceService::AbstractResourceService(ApplicationContext &context)
        : AbstractRuntimeComponent(context), vulkanContext(context.getVulkanContext()) {
    }
}
