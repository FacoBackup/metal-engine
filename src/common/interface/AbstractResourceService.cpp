#include "AbstractResourceService.h"

#include "../../context/repository/ResourceRepository.h"
#include "../../context/ApplicationContext.h"

namespace Metal {
    void AbstractResourceService::registerResource( RuntimeResource *resource) const {
        repository.resources.insert({resource->id, resource});
    }

    AbstractResourceService::AbstractResourceService(ApplicationContext &context)
        : AbstractRuntimeComponent(context), vulkanContext(context.getVulkanContext()),
          repository(context.getEngineContext().getResourceRepository()) {
    }
}
