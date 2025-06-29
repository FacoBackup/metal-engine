#include "TextureService.h"

#include <iostream>

#include "../../common/enum/LevelOfDetail.h"
#include "../util/ImageUtils.h"

#include <stb_image.h>
#include <string>

#include "../data/TextureData.h"
#include "../../MetalContextProvider.h"
#include "../data/TextureInstance.h"
#include "../data/BufferInstance.h"

namespace Metal {
    void TextureService::copyBufferToImage(const VkBuffer &vkBuffer, const TextureInstance *image,
                                           const int layerCount) const {
        VkCommandBuffer commandBuffer = SINGLETONS.vulkanContext.beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {static_cast<unsigned int>(image->width), static_cast<unsigned int>(image->height), 1};

        vkCmdCopyBufferToImage(
            commandBuffer,
            vkBuffer,
            image->vkImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);
        SINGLETONS.vulkanContext.endSingleTimeCommands(commandBuffer);
    }

    void TextureService::createImageWithInfo(const VkImageCreateInfo &imageInfo,
                                             VkMemoryPropertyFlagBits vkMemoryProperties,
                                             TextureInstance *image) const {
        if (vkCreateImage(SINGLETONS.vulkanContext.device.device, &imageInfo, nullptr, &image->vkImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(SINGLETONS.vulkanContext.device.device, image->vkImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = SINGLETONS.bufferService.findMemoryType(
            memRequirements.memoryTypeBits, vkMemoryProperties);

        if (vkAllocateMemory(SINGLETONS.vulkanContext.device.device, &allocInfo, nullptr, &image->vkImageMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        if (vkBindImageMemory(SINGLETONS.vulkanContext.device.device, image->vkImage, image->vkImageMemory, 0) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to bind image memory!");
        }
    }

    void TextureService::createSampler(TextureInstance *image) const {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(image->mipLevels);
        samplerInfo.maxAnisotropy = 16.0;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        vkCreateSampler(SINGLETONS.vulkanContext.device.device, &samplerInfo, nullptr, &image->vkSampler);
    }

    void TextureService::createImageView(VkFormat imageFormat, TextureInstance *image) const {
        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = imageFormat;
        imageViewInfo.components = {
            VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A
        };
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.subresourceRange.levelCount = image->mipLevels;
        imageViewInfo.image = image->vkImage;

        vkCreateImageView(SINGLETONS.vulkanContext.device.device, &imageViewInfo, nullptr, &image->vkImageView);
    }

    void TextureService::createImage(VkFormat imageFormat, TextureInstance *image, const int width,
                                     const int height) const {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = imageFormat;
        imageInfo.mipLevels = image->mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.extent = {static_cast<unsigned int>(width), static_cast<unsigned int>(height), 1};
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                          VK_IMAGE_USAGE_SAMPLED_BIT;

        createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image);
    }

    TextureData *TextureService::stream(const std::string &id, const LevelOfDetail &lod) const {
        auto pathToFile = ContextProvider::Get().getAssetDirectory() + FORMAT_FILE_TEXTURE(id, lod);
        if (std::filesystem::exists(pathToFile)) {
            int width, height, channels;
            unsigned char *data = stbi_load(pathToFile.c_str(), &width, &height, &channels, 0);
            if (!data) {
                throw std::runtime_error("Failed to load image: " + pathToFile);
            }
            return new TextureData{width, height, channels, data};
        }
        return nullptr;
    }

    TextureInstance *TextureService::loadTexture(const std::string &id, const std::string &pathToImage,
                                                 bool generateMipMaps,
                                                 VkFormat imageFormat) {
        auto *image = new TextureInstance(id);
        registerResource(image);
        int width, height, channels;
        const auto data = stbi_load(pathToImage.c_str(), &width, &height, &channels, 4);
        image->width = width;
        image->height = height;
        image->channels = channels;
        image->mipLevels = generateMipMaps
                               ? static_cast<unsigned int>(std::floor(std::log2(std::max(image->width, image->height)))) + 1
                               : 1;
        image->vkFormat = imageFormat;
        std::cout << "Loading texture " << id << " from " << pathToImage << std::endl;
        std::cout << "Texture data: Width " << image->width << " Height " << image->height << std::endl;
        const std::shared_ptr<BufferInstance> stagingBuffer = SINGLETONS.bufferService.createBuffer(
            image->width * image->height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer->update(data);

        createImage(imageFormat, image, image->width, image->height);
        transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer->vkBuffer, image, 1);
        if (image->mipLevels > 1) {
            generateMipmaps(image);
        }
        createSampler(image);
        createImageView(imageFormat, image);

        stbi_image_free(data);
        return image;
    }

    void TextureService::transitionImageLayout(const TextureInstance *image, VkImageLayout oldLayout,
                                               VkImageLayout newLayout) const {
        VkCommandBuffer commandBuffer = SINGLETONS.vulkanContext.beginSingleTimeCommands();
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image->vkImage;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = image->mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout ==
                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        SINGLETONS.vulkanContext.endSingleTimeCommands(commandBuffer);
    }

    void TextureService::generateMipmaps(const TextureInstance *image) const {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(SINGLETONS.vulkanContext.physDevice.physical_device, image->vkFormat,
                                            &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = SINGLETONS.vulkanContext.beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image->vkImage;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = image->width;
        int32_t mipHeight = image->height;

        for (unsigned int i = 1; i < image->mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer, image->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image->vkImage,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0, 0, nullptr, 0, nullptr, 1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = image->mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier);

        SINGLETONS.vulkanContext.endSingleTimeCommands(commandBuffer);
    }

    TextureInstance *TextureService::create(const std::string &id, const LevelOfDetail &lod) {
        auto pathToFile = ContextProvider::Get().getAssetDirectory() + FORMAT_FILE_TEXTURE(id, lod);
        if (std::filesystem::exists(pathToFile)) {
            return loadTexture(id + lod.suffix, pathToFile, true, VK_FORMAT_R8G8B8A8_UNORM);
        }
        return nullptr;
    }

    TextureInstance *TextureService::createForCompute(const unsigned int width, const unsigned int height) {
        auto *image = new TextureInstance(Util::uuidV4());
        registerResource(image);
        image->width = width;
        image->height = height;
        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // No multisampling
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        createImageWithInfo(imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image);
        createImageView(VK_FORMAT_R16G16B16A16_SFLOAT, image);

        transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        return image;
    }
} // Metal
