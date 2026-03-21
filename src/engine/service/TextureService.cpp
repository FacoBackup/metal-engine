#include "TextureService.h"

#include <iostream>
#include <filesystem>

#include "../util/ImageUtils.h"
#include "../../editor/enum/engine-definitions.h"
#include "../../ApplicationContext.h"

#include <stb_image.h>
#include <string>

#include "../dto/TextureData.h"
#include "../../core/VulkanContext.h"
#include "../../common/VulkanUtils.h"
#include "../resource/TextureInstance.h"
#include "../resource/BufferInstance.h"
#include "BufferService.h"
#include "PipelineService.h"
#include "DescriptorSetService.h"
#include "../../core/DirectoryService.h"
#include "../../common/LoggerUtil.h"

namespace Metal {

    void TextureService::copyBufferToImage(const VkBuffer &vkBuffer, const TextureInstance *image,
                                           const int layerCount) const {
        VkCommandBuffer commandBuffer = vulkanContext->beginSingleTimeCommands();

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
        vulkanContext->endSingleTimeCommands(commandBuffer);
    }

    void TextureService::createImageWithInfo(const VkImageCreateInfo &imageInfo,
                                             VkMemoryPropertyFlagBits vkMemoryProperties,
                                             TextureInstance *image) const {
        if (vkCreateImage(vulkanContext->device.device, &imageInfo, nullptr, &image->vkImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(vulkanContext->device.device, image->vkImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = bufferService->findMemoryType(
            memRequirements.memoryTypeBits, vkMemoryProperties);

        if (vkAllocateMemory(vulkanContext->device.device, &allocInfo, nullptr, &image->vkImageMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        if (vkBindImageMemory(vulkanContext->device.device, image->vkImage, image->vkImageMemory, 0) !=
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

        vkCreateSampler(vulkanContext->device.device, &samplerInfo, nullptr, &image->vkSampler);
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

        vkCreateImageView(vulkanContext->device.device, &imageViewInfo, nullptr, &image->vkImageView);
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

    TextureData *TextureService::loadTextureData(const std::string &pathToFile) const {
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

    TextureInstance *TextureService::stream(const std::string &id) {
        if (id.empty()) {
            return nullptr;
        }
        auto *resource = getResource(id);
        if (resource != nullptr) {
            return resource;
        }
        return create(id);
    }

    TextureInstance *TextureService::loadTexture(const std::string &id, const std::string &pathToImage,
                                                 bool generateMipMaps,
                                                 VkFormat imageFormat) {
        auto *image = createResourceInstance(id);
        int width, height, channels;
        const auto data = stbi_load(pathToImage.c_str(), &width, &height, &channels, 4);
        image->width = width;
        image->height = height;
        image->channels = channels;
        image->mipLevels = generateMipMaps
                               ? static_cast<unsigned int>(std::floor(std::log2(std::max(image->width, image->height)))) + 1
                               : 1;
        image->vkFormat = imageFormat;
        LOG_INFO("Loading texture " + id + " from " + pathToImage);
        LOG_INFO("Texture data: Width " + std::to_string(image->width) + " Height " + std::to_string(image->height));
        BufferInstance *stagingBuffer = bufferService->createBuffer(
            id + "_staging",
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
        VkCommandBuffer commandBuffer = vulkanContext->beginSingleTimeCommands();
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

        vulkanContext->endSingleTimeCommands(commandBuffer);
    }

    void TextureService::generateMipmaps(const TextureInstance *image) const {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(vulkanContext->physDevice.physical_device, image->vkFormat,
                                            &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = vulkanContext->beginSingleTimeCommands();

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

        vulkanContext->endSingleTimeCommands(commandBuffer);
    }

    TextureInstance *TextureService::create(const std::string &pathToFile) {
        if (std::filesystem::exists(pathToFile)) {
            auto *instance = loadTexture(pathToFile, pathToFile, true, VK_FORMAT_R8G8B8A8_UNORM);
            if (instance != nullptr) {
                getTextureIndex(pathToFile);
            }
            return instance;
        }
        return nullptr;
    }

    TextureInstance *TextureService::createForCompute(const std::string &id, const unsigned int width, const unsigned int height, VkFormat format) {
        auto *image = createResourceInstance(id);
        image->width = width;
        image->height = height;
        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = format;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // No multisampling
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        createImageWithInfo(imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image);
        createImageView(format, image);

        transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        return image;
    }

    unsigned int TextureService::getTextureIndex(const std::string &id) {
        if (id.empty()) return 0;
        if (textureIndices.contains(id)) return textureIndices[id];

        if (resources.contains(id)) {
            auto *texture = resources.at(id);
            unsigned int index = nextTextureIndex++;
            textureIndices[id] = index;

            // Update all descriptors that contain a texture array
            auto descriptors = pipelineService->getAllDescriptors();
            for (auto *descriptor: descriptors) {
                for (auto &binding: descriptor->bindings) {
                    if (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && binding.descriptorCount > 1) {
                        VkWriteDescriptorSet write{};
                        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        write.dstSet = descriptor->vkDescriptorSet;
                        write.dstBinding = binding.bindingPoint;
                        write.dstArrayElement = index;
                        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        write.descriptorCount = 1;

                        VkDescriptorImageInfo imageInfo{};
                        imageInfo.sampler = vulkanContext->vkTextureSampler;
                        imageInfo.imageView = texture->vkImageView;
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        write.pImageInfo = &imageInfo;

                        vkUpdateDescriptorSets(vulkanContext->device.device, 1, &write, 0, nullptr);
                    }
                }
            }

            return index;
        }
        return 0;
    }

    void TextureService::disposeResource(TextureInstance *resource) {
        vkDestroyImage(vulkanContext->device.device, resource->vkImage, nullptr);
        vkFreeMemory(vulkanContext->device.device, resource->vkImageMemory, nullptr);
        vkDestroyImageView(vulkanContext->device.device, resource->vkImageView, nullptr);
        vkDestroySampler(vulkanContext->device.device, resource->vkSampler, nullptr);

        if (resource->imageDescriptor != nullptr) {
            descriptorSetService->disposeResource(resource->imageDescriptor);
        }
    }

    void TextureService::createSampler(bool linear, VkSampler &vkImageSampler, VkSamplerAddressMode addressMode) {
        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.magFilter = linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        samplerCreateInfo.minFilter = linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        samplerCreateInfo.mipmapMode = linear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerCreateInfo.addressModeU = addressMode;
        samplerCreateInfo.addressModeV = addressMode;
        samplerCreateInfo.addressModeW = addressMode;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1;
        // TODO - ENABLE/DISABLE ANISOTROPY
        samplerCreateInfo.maxAnisotropy = 8;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        VulkanUtils::CheckVKResult(vkCreateSampler(vulkanContext->device.device, &samplerCreateInfo, nullptr,
                                                   &vkImageSampler));
    }

} // Metal
