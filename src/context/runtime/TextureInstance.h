#ifndef TEXTUREINSTANCE_H
#define TEXTUREINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "RuntimeResource.h"
#include "../VulkanContext.h"

namespace Metal {
    struct TextureInstance final : RuntimeResource {
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkSampler vkSampler = VK_NULL_HANDLE;

        void dispose(VulkanContext &context) override {
            vkDestroyImageView(context.device.device, vkImageView, nullptr);
            vkDestroyImage(context.device.device, image, nullptr);
        }
    };
} // Metal

#endif //TEXTUREINSTANCE_H
