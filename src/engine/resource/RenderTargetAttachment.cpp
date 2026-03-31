#include "RenderTargetAttachment.h"
#include "../service/DescriptorSetService.h"
#include "../../core/VulkanContext.h"

namespace Metal {
    void RenderTargetAttachment::dispose() const {
        vkDestroyImage(vulkanContext->device.device, vkImage, nullptr);
        vkDestroyImageView(vulkanContext->device.device, vkImageView, nullptr);
        vkFreeMemory(vulkanContext->device.device, vkImageMemory, nullptr);
    }
}
