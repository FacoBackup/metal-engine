#include "CoreFrameBuffers.h"
#include "../../context/ApplicationContext.h"


namespace Metal {
    void CoreFrameBuffers::onInitialize() { {
            // G-BUFFER
            gBufferFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                              vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Albedo Emission", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Roughness Metallic AO", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Normal", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                gBufferFBO);
            framebufferService.createAttachment("Depth ID UV", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createDepthAttachment(gBufferFBO);
            framebufferService.createRenderPass(gBufferFBO);
        } {
            // AUX FRAME BUFFER
            auxFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                          vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Color", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, auxFBO);
            framebufferService.createRenderPass(auxFBO);
        } {
            globalIlluminationFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth() / 2,
                                                         vulkanContext.getWindowHeight() / 2, glm::vec4(1));
            framebufferService.createAttachment("Ambient occlusion", VK_FORMAT_R16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, globalIlluminationFBO);
            framebufferService.createRenderPass(globalIlluminationFBO);
        } {
            // POST PROCESSING
            postProcessingFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                                     vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Color", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, postProcessingFBO);
            framebufferService.createRenderPass(postProcessingFBO);
        }
    }
}
