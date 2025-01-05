#include "CoreFrameBuffers.h"
#include "../../context/ApplicationContext.h"


namespace Metal {
    void CoreFrameBuffers::onInitialize() { {
            // G-BUFFER
            gBufferFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                              vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Albedo; Emission flag", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Normal; AO", VK_FORMAT_R16G16B16A16_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createAttachment("Depth; ID; Roughness; Metallic", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferFBO);
            framebufferService.createDepthAttachment(gBufferFBO);
            framebufferService.createRenderPass(gBufferFBO);
        } {
            rayGenFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                             vulkanContext.getWindowHeight(), glm::vec4(0, 0, 0, 0));
            framebufferService.createAttachment("Voxel Positions", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, rayGenFBO);
            framebufferService.createAttachment("Hit positions", VK_FORMAT_R32G32B32A32_SFLOAT,
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, rayGenFBO);
            framebufferService.createRenderPass(rayGenFBO);
        } {
            // AUX FRAME BUFFER
            auxFBO = framebufferService.createFrameBuffer(vulkanContext.getWindowWidth(),
                                                          vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Color", VK_FORMAT_R16G16B16A16_SFLOAT,
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
