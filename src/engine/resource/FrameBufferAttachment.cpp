#include "FrameBufferAttachment.h"
#include "../service/DescriptorSetService.h"
#include "../../core/vulkan/VulkanContext.h"

namespace Metal {
    void FrameBufferAttachment::dispose() const {
        vkDestroyImage(vulkanContext->device.device, vkImage, nullptr);
        vkDestroyImageView(vulkanContext->device.device, vkImageView, nullptr);
        vkFreeMemory(vulkanContext->device.device, vkImageMemory, nullptr);

        if (imageDescriptor != nullptr) {
            descriptorSetService->disposeResource(imageDescriptor);
        }
    }
}
