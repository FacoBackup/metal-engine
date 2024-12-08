#include "AbstractResourceService.h"

#include "../repository/resource/ResourceRepository.h"
#include "../../common/runtime/ApplicationContext.h"

namespace Metal {
    void AbstractResourceService::registerResource( RuntimeResource *resource) const {
        repository.resources.insert({resource->id, resource});
    }

    AbstractResourceService::AbstractResourceService(ApplicationContext &context)
        : AbstractRuntimeComponent(context), vulkanContext(context.getVulkanContext()),
          repository(context.getEngineContext().getResourceRepository()) {
    }
}
