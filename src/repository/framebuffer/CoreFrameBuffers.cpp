#include "CoreFrameBuffers.h"
#include "../../context/ApplicationContext.h"


namespace Metal {
    void CoreFrameBuffers::onInitialize() {
        postProcessingFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                                 vulkanContext.getWindowHeight());
        framebufferService.createAttachment("Color", VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, postProcessingFBO);
        framebufferService.createRenderPass(postProcessingFBO);
    }
}
