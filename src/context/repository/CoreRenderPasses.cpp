#include "CoreRenderPasses.h"

#include "../VulkanContext.h"
#include "../repository/CoreFrameBuffers.h"
#include "../runtime/RenderPass.h"

namespace Metal {
    void CoreRenderPasses::onInitialize() {
        gBufferPass = new RenderPass(vulkanContext.coreFrameBuffers.gBufferFBO, context);
        fullScreenPass = new RenderPass(vulkanContext.coreFrameBuffers.auxFBO, context);
        postProcessingPass = new RenderPass(vulkanContext.coreFrameBuffers.postProcessingFBO, context);
    }
} // Metal