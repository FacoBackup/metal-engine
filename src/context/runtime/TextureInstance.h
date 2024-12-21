#ifndef TEXTUREINSTANCE_H
#define TEXTUREINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "DescriptorInstance.h"
#include "RuntimeResource.h"
#include "../VulkanContext.h"

namespace Metal {
    struct TextureInstance final : RuntimeResource {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkSampler vkSampler = VK_NULL_HANDLE;
        uint32_t mipLevels = 1;
        int width{}, height{}, channels{};
        VkFormat vkFormat{};
        std::unique_ptr<DescriptorInstance> imageDescriptor = nullptr;

        explicit TextureInstance(const std::string& id) : RuntimeResource(id) {
        }

        void dispose(VulkanContext &context) override {
            vkDestroyImage(context.device.device, vkImage, nullptr);
            vkFreeMemory(context.device.device, vkImageMemory, nullptr);
            vkDestroyImageView(context.device.device, vkImageView, nullptr);
            vkDestroySampler(context.device.device, vkSampler, nullptr);

            if (imageDescriptor != nullptr) {
                imageDescriptor->dispose(context);
            }
        }

        ResourceType resourceType() override {
            return TEXTURE;
        }
    };
} // Metal

#endif //TEXTUREINSTANCE_H
