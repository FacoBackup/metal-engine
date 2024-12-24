#include "CoreRenderPasses.h"

#include "../../context/ApplicationContext.h"
#include "../../repository/framebuffer/CoreFrameBuffers.h"
#include "../../context/engine/CommandBufferRecorder.h"

namespace Metal {
    void CoreRenderPasses::onInitialize() {
        gBufferPass = new CommandBufferRecorder(context.coreFrameBuffers.gBufferFBO, context);
        fullScreenPass = new CommandBufferRecorder(context.coreFrameBuffers.auxFBO, context);
        postProcessingPass = new CommandBufferRecorder(context.coreFrameBuffers.postProcessingFBO, context);
    }
} // Metal