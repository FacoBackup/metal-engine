#include "../../repository/abstract/AbstractCoreRepository.h"
#include "../../context/ApplicationContext.h"

namespace Metal {
    AbstractCoreRepository::AbstractCoreRepository(ApplicationContext &context)
        : AbstractRuntimeComponent(context),
          vulkanContext(context.vulkanContext),
          framebufferService(context.framebufferService),
          bufferService(context.bufferService),
          pipelineService(context.pipelineService){
    }
}
