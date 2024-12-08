#include "CoreRenderPasses.h"

#include "../../common/runtime/ApplicationContext.h"

namespace Metal {
    void CoreRenderPasses::onInitialize() { {
            // G-Buffer
            gBufferRenderPass = framebufferService.createPipeline(vulkanContext.getWindowWidth(),
                                                             vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Albedo Emission", VK_FORMAT_R16G16B16A16_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferRenderPass);
            framebufferService.createAttachment("Roughness Metallic AO", VK_FORMAT_R16G16B16A16_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferRenderPass);
            framebufferService.createAttachment("Normal", VK_FORMAT_R16G16B16A16_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                             gBufferRenderPass);
            framebufferService.createAttachment("Depth ID UV", VK_FORMAT_R32G32B32A32_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferRenderPass);
            framebufferService.createDepthAttachment(gBufferRenderPass);
            framebufferService.createRenderPass(gBufferRenderPass);
            framebufferService.createFrameBuffer(gBufferRenderPass);
        } {
            // Aux-Buffer
            auxRenderPass = framebufferService.createPipeline(vulkanContext.getWindowWidth(),
                                                               vulkanContext.getWindowHeight());
            framebufferService.createAttachment("Color", VK_FORMAT_R32G32B32A32_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, auxRenderPass);
            framebufferService.createDepthAttachment(auxRenderPass);
            framebufferService.createRenderPass(auxRenderPass);
            framebufferService.createFrameBuffer(auxRenderPass);
        }
    }
}
