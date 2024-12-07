#include "TextureService.h"
#include "vk_mem_alloc.h"
#include "../../../common/runtime/VulkanContext.h"

namespace Metal {
    TextureInstance *TextureService::createTextureInstance(const int w, const int h, const VkFormat format, const bool isLinear) const {
        auto *textureInstance = new TextureInstance();

        // Create the off-screen image
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent.width = w;
        imageInfo.extent.height = h;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = isLinear ? VK_IMAGE_TILING_LINEAR  : VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        // Allocate memory using VMA
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VmaAllocation offscreenImageAllocation;
        vmaCreateImage(vulkanContext.allocator, &imageInfo, &allocInfo, &textureInstance->image,
                       &offscreenImageAllocation, nullptr);

        // Create an image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = textureInstance->image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(vulkanContext.device.device, &viewInfo, nullptr, &textureInstance->imageView);

        registerResource(textureInstance);
        return textureInstance;
    }
} // Metal
