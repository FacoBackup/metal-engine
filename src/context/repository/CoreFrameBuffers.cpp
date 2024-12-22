#include "CoreFrameBuffers.h"

#include "../ApplicationContext.h"

namespace Metal {
    void CoreFrameBuffers::onInitialize() { {
            // G-BUFFER
            gBufferFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                              vulkanContext.getWindowHeight());
            framebufferService.createDepthAttachment(gBufferFBO);
            framebufferService.createAttachment("Albedo Emission", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Roughness Metallic AO", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Normal", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                gBufferFBO);
            framebufferService.createAttachment("Depth ID UV", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createRenderPass(gBufferFBO);
        } {
            // AUX FRAME BUFFER
            auxFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                          vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Color", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, auxFBO);
            framebufferService.createRenderPass(auxFBO);
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
