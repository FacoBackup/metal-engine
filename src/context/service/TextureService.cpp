#include "TextureService.h"

#include <stb_image.h>
#include <string>
#include "../runtime/TextureInstance.h"

namespace Metal {
    TextureInstance *TextureService::loadTexture(const std::string &pathToImage)  {
        auto *image = new TextureInstance;
        registerResource(image);

        int width, height, channels;
        unsigned char *data = stbi_load(pathToImage.c_str(), &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load image: " + pathToImage);
        }
        // Create the Vulkan image
        createVkImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      image->image, image->imageMemory);

        image->vkImageView = createVkImageView(image->image, VK_FORMAT_R8G8B8A8_SRGB);
        image->vkSampler = createVkSampler();

        return image;
    }

    void TextureService::createVkImage(const uint32_t width, const uint32_t height,
                                       const VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                                       VkMemoryPropertyFlags properties, VkImage &image,
                                       VkDeviceMemory &imageMemory) const {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(vulkanContext.device.device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(vulkanContext.device.device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;

        allocInfo.memoryTypeIndex = vulkanContext.bufferService.findMemoryType(
            memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(vulkanContext.device.device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory!");
        }

        vkBindImageMemory(vulkanContext.device.device, image, imageMemory, 0);
    }

    VkImageView TextureService::createVkImageView(VkImage image, VkFormat format) const {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(vulkanContext.device.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }

        return imageView;
    }

    VkSampler TextureService::createVkSampler() const {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        VkSampler sampler;
        if (vkCreateSampler(vulkanContext.device.device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sampler!");
        }

        return sampler;
    }
} // Metal
