#include "AbstractCoreRepository.h"
#include "../../common/runtime/ApplicationContext.h"

namespace Metal {
    AbstractCoreRepository::AbstractCoreRepository(ApplicationContext &context)
        : AbstractRuntimeComponent(context),
          vulkanContext(context.getVulkanContext()),
          framebufferService(context.getEngineContext().framebufferService),
          shaderService(context.getEngineContext().shaderService),
          bufferService(context.getEngineContext().bufferService),
          pipelineService(context.getEngineContext().pipelineService),
          commandPoolService(context.getEngineContext().poolService) {
    }
}
