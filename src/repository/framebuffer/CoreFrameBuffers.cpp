#include "CoreFrameBuffers.h"
#include "../../context/ApplicationContext.h"


namespace Metal {
    void CoreFrameBuffers::onInitialize() {
        auto &framebufferService = CTX.framebufferService;

        {
            // G-BUFFER
            gBufferFBO = framebufferService.createFrameBuffer(
                CTX.vulkanContext.getWindowWidth() / CTX.engineRepository.shadingResInvScale,
                CTX.vulkanContext.getWindowHeight() / CTX.engineRepository.shadingResInvScale, glm::vec4(0, 0, 0, 0));
            framebufferService.createAttachment("Albedo; Emission flag | AO", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Normal; Roughness | Metallic", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Position; ID", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createDepthAttachment(gBufferFBO);
            framebufferService.createRenderPass(gBufferFBO);
        } {
            // POST PROCESSING
            postProcessingFBO = framebufferService.createFrameBuffer(CTX.vulkanContext.getWindowWidth(),
                                                                     CTX.vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Color", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, postProcessingFBO);
            framebufferService.createRenderPass(postProcessingFBO);
        }
    }
}
