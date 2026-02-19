#include "CoreFrameBuffers.h"
#include "../../context/ApplicationContext.h"


namespace Metal {
    void CoreFrameBuffers::onInitialize() {
        auto &framebufferService = ApplicationContext::Get().framebufferService;

        {
            // G-BUFFER
            gBufferFBO = framebufferService.createFrameBuffer(
                ApplicationContext::Get().vulkanContext.getWindowWidth() / ApplicationContext::Get().engineRepository.shadingResInvScale,
                ApplicationContext::Get().vulkanContext.getWindowHeight() / ApplicationContext::Get().engineRepository.shadingResInvScale, glm::vec4(0, 0, 0, 0));
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
            postProcessingFBO = framebufferService.createFrameBuffer(ApplicationContext::Get().vulkanContext.getWindowWidth(),
                                                                     ApplicationContext::Get().vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Color", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, postProcessingFBO);
            framebufferService.createRenderPass(postProcessingFBO);
        }
    }
}
