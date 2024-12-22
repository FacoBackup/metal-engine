#include "AbstractCoreRepository.h"
#include "../ApplicationContext.h"

namespace Metal {
    AbstractCoreRepository::AbstractCoreRepository(ApplicationContext &context)
        : AbstractRuntimeComponent(context),
          vulkanContext(context.vulkanContext),
          framebufferService(context.vulkanContext.framebufferService),
          bufferService(context.vulkanContext.bufferService),
          pipelineService(context.vulkanContext.pipelineService){
    }
}
