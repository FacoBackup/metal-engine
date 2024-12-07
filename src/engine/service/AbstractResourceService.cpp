#include "AbstractResourceService.h"

#include "../repository/resource/ResourceRepository.h"
#include "../../common/runtime/ApplicationContext.h"

namespace Metal {
    void AbstractResourceService::registerResource(const RuntimeResource *resource) const {
        repository.resources.insert_or_assign(resource->id, repository);
    }

    AbstractResourceService::AbstractResourceService(ApplicationContext &context)
        : AbstractRuntimeComponent(context), vulkanContext(context.getVulkanContext()),
          repository(context.getEngineContext().getResourceRepository()) {
    }
}
