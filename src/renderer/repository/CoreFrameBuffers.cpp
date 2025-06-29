#include "CoreFrameBuffers.h"
#include "../../MetalContextProvider.h"


namespace Metal {
    void CoreFrameBuffers::onInitialize() {
        postProcessingFBO = SINGLETONS.framebufferService.createFrameBuffer(SINGLETONS.vulkanContext.getWindowWidth(),
                                                                 SINGLETONS.vulkanContext.getWindowHeight());
        SINGLETONS.framebufferService.createAttachment("Color", VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, postProcessingFBO);
        SINGLETONS.framebufferService.createRenderPass(postProcessingFBO);
    }
}
