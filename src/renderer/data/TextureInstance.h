#ifndef TEXTUREINSTANCE_H
#define TEXTUREINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "DescriptorInstance.h"
#include "../../common/RuntimeResource.h"
#include "../VulkanContext.h"

namespace Metal {
    struct TextureInstance final : RuntimeResource {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkSampler vkSampler = VK_NULL_HANDLE;
        unsigned int  mipLevels = 1;
        unsigned int  width{}, height{}, channels{};
        VkFormat vkFormat{};
        std::unique_ptr<DescriptorInstance> imageDescriptor = nullptr;

        explicit TextureInstance(const std::string& id) : RuntimeResource(id) {
        }

        void dispose() override {
            vkDestroyImage(SINGLETONS.vulkanContext.device.device, vkImage, nullptr);
            vkFreeMemory(SINGLETONS.vulkanContext.device.device, vkImageMemory, nullptr);
            vkDestroyImageView(SINGLETONS.vulkanContext.device.device, vkImageView, nullptr);
            vkDestroySampler(SINGLETONS.vulkanContext.device.device, vkSampler, nullptr);

            if (imageDescriptor != nullptr) {
                imageDescriptor->dispose();
            }
        }

        ResourceType resourceType() override {
            return TEXTURE;
        }
    };
} // Metal

#endif //TEXTUREINSTANCE_H
