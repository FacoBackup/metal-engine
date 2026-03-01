#ifndef TEXTUREINSTANCE_H
#define TEXTUREINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../descriptor/DescriptorInstance.h"
#include "../../repository/abstract/RuntimeResource.h"
#include "../../context/vulkan/VulkanContext.h"

namespace Metal {
    struct TextureInstance final : RuntimeResource {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkSampler vkSampler = VK_NULL_HANDLE;
        unsigned int mipLevels = 1;
        unsigned int width{}, height{}, channels{};
        VkFormat vkFormat{};
        DescriptorInstance *imageDescriptor = nullptr;

        explicit TextureInstance(const std::string& id) : RuntimeResource(id) {
        }

        ResourceType resourceType() override {
            return TEXTURE;
        }
    };
} // Metal

#endif //TEXTUREINSTANCE_H
