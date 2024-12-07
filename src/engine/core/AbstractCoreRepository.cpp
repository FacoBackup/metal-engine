#include "AbstractCoreRepository.h"
#include "../../common/runtime/ApplicationContext.h"

namespace Metal {
    AbstractCoreRepository::AbstractCoreRepository(ApplicationContext &context)
        : AbstractRuntimeComponent(context),
          vulkanContext(context.getVulkanContext()),
          renderPassService(context.getEngineContext().getRenderPassService()),
          shaderService(context.getEngineContext().getShaderService()) {
    }
}
