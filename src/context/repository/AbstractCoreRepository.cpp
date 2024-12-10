#include "AbstractCoreRepository.h"
#include "../ApplicationContext.h"

namespace Metal {
    AbstractCoreRepository::AbstractCoreRepository(ApplicationContext &context)
        : AbstractRuntimeComponent(context),
          vulkanContext(context.getVulkanContext()),
          framebufferService(context.getVulkanContext().framebufferService),
          shaderService(context.getVulkanContext().shaderService),
          bufferService(context.getVulkanContext().bufferService),
          pipelineService(context.getVulkanContext().pipelineService),
          commandPoolService(context.getVulkanContext().commandService) {
    }
}
