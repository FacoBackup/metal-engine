#include "CoreRenderPasses.h"

#include "../VulkanContext.h"
#include "../repository/CoreFrameBuffers.h"
#include "../runtime/RenderPass.h"

namespace Metal {
    void CoreRenderPasses::onInitialize() {
        fullScreenPass = new RenderPass(vulkanContext.coreFrameBuffers.auxRenderPass, context);
    }
} // Metal