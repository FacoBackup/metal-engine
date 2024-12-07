#include "FrameBufferService.h"
#include "../../render-pass/AbstractRenderPass.h"

#include <vulkan/vulkan_core.h>
#include "../../../common/runtime/VulkanContext.h"
#include "../../../common/runtime/ApplicationContext.h"

namespace Metal {
    FrameBufferService::FrameBufferService(ApplicationContext &context)
        : AbstractResourceService(context), textureService(context.getEngineContext().getTextureService()) {
    }

    FramebufferInstance *FrameBufferService::createFrameBuffer(AbstractRenderPass &renderPass) {
        auto *instance = new FramebufferInstance;
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.getRenderPass();
        framebufferInfo.attachmentCount = 1;
        // TODO - Multiple attachments
        framebufferInfo.pAttachments = &offscreenImageView;
        framebufferInfo.width = imageInfo.extent.width;
        framebufferInfo.height = imageInfo.extent.height;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(vulkanContext.device.device, &framebufferInfo, vulkanContext.instance.allocation_callbacks, &instance->framebuffer);
        registerResource(instance);
        return instance;
    }
} // Metal
