#include "CoreRenderPasses.h"

#include "../VulkanContext.h"
#include "../repository/CoreFrameBuffers.h"
#include "../runtime/CommandBufferRecorder.h"

namespace Metal {
    void CoreRenderPasses::onInitialize() {
        gBufferPass = new CommandBufferRecorder(vulkanContext.coreFrameBuffers.gBufferFBO, context);
        fullScreenPass = new CommandBufferRecorder(vulkanContext.coreFrameBuffers.auxFBO, context);
        postProcessingPass = new CommandBufferRecorder(vulkanContext.coreFrameBuffers.postProcessingFBO, context);
    }
} // Metal