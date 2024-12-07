#include "CoreRenderPasses.h"

#include "../../common/runtime/VulkanContext.h"
#include "../service/render-pass/RenderPassService.h"
#include "../service/render-pass/RenderPassInstance.h"

namespace Metal {
    void CoreRenderPasses::onInitialize() { {
            // G-Buffer
            gBufferRenderPass = renderPassService.createPipeline(vulkanContext.getWindowWidth(),
                                                             vulkanContext.getWindowHeight());
            renderPassService.createAttachment("Albedo Emission", VK_FORMAT_R16G16B16A16_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferRenderPass);
            renderPassService.createAttachment("Roughness Metallic AO", VK_FORMAT_R16G16B16A16_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferRenderPass);
            renderPassService.createAttachment("Normal", VK_FORMAT_R16G16B16A16_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                             gBufferRenderPass);
            renderPassService.createAttachment("Depth ID UV", VK_FORMAT_R32G32B32A32_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, gBufferRenderPass);
            renderPassService.createDepthAttachment(gBufferRenderPass);
            renderPassService.createRenderPass(gBufferRenderPass);
            renderPassService.createFrameBuffer(gBufferRenderPass);
        } {
            // Aux-Buffer
            auxRenderPass = renderPassService.createPipeline(vulkanContext.getWindowWidth(),
                                                               vulkanContext.getWindowHeight());
            renderPassService.createAttachment("Color", VK_FORMAT_R32G32B32A32_SFLOAT,
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, auxRenderPass);
            renderPassService.createDepthAttachment(auxRenderPass);
            renderPassService.createRenderPass(auxRenderPass);
            renderPassService.createFrameBuffer(auxRenderPass);
        }
    }
}
