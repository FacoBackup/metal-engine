#include "RenderTargetService.h"

#include <array>
#include <glm/vec3.hpp>

#include "../resource/RenderTargetInstance.h"
#include "../../core/VulkanContext.h"
#include "../../common/VulkanUtils.h"
#include "../resource/RenderTargetAttachment.h"
#include "../../ApplicationContext.h"
#include "../../common/LoggerUtil.h"

namespace Metal {

    RenderTargetInstance *RenderTargetService::createRenderTarget(const std::string &id, const unsigned w,
                                                               const unsigned h, glm::vec4 clearColor) {
        auto *framebuffer = createResourceInstance(id);
        framebuffer->bufferWidth = w;
        framebuffer->bufferHeight = h;
        framebuffer->clearColor = clearColor;

        return framebuffer;
    }

    void RenderTargetService::createDepthAttachment(RenderTargetInstance *framebuffer) const {
        VkFormat depthFormat = VulkanUtils::GetValidDepthFormat(vulkanContext->physDevice.physical_device);
        const auto att = createAttachmentInternal(depthFormat,
                                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, framebuffer);
        att->depth = true;
    }

    void RenderTargetService::createAttachment(VkFormat format, VkImageUsageFlags usage,
                                              RenderTargetInstance *framebuffer) const {
        const auto att = createAttachmentInternal(format,
                                                  usage,
                                                  framebuffer);
        att->depth = false;
    }

    std::shared_ptr<RenderTargetAttachment> RenderTargetService::createAttachmentInternal(
        VkFormat format,
        VkImageUsageFlags usage,
        RenderTargetInstance *framebuffer) const {
        std::shared_ptr<RenderTargetAttachment> attachment = std::make_shared<RenderTargetAttachment>();
        ctx->injectDependencies(attachment.get());
        framebuffer->attachments.push_back(attachment);
        attachment->format = format;

        VkImageAspectFlags aspectMask = 0;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
            aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        assert(aspectMask > 0);

        VkImageCreateInfo image{};
        image.imageType = VK_IMAGE_TYPE_2D;
        image.format = format;
        image.extent.width = framebuffer->bufferWidth;
        image.extent.height = framebuffer->bufferHeight;
        image.extent.depth = 1;
        image.mipLevels = 1;
        image.arrayLayers = 1;
        image.samples = VK_SAMPLE_COUNT_1_BIT;
        image.tiling = VK_IMAGE_TILING_OPTIMAL;
        image.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;
        image.initialLayout = layout;
        image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanUtils::CheckVKResult(
            vkCreateImage(vulkanContext->device.device, &image, nullptr, &attachment->vkImage));

        VkMemoryAllocateInfo memAlloc{};
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(vulkanContext->device.device, attachment->vkImage, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = VulkanUtils::GetMemTypeIndex(vulkanContext->physicalDeviceMemoryProperties,
                                                                memReqs.memoryTypeBits,
                                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VulkanUtils::CheckVKResult(vkAllocateMemory(vulkanContext->device.device, &memAlloc, nullptr,
                                                    &attachment->vkImageMemory));
        VulkanUtils::CheckVKResult(
            vkBindImageMemory(vulkanContext->device.device, attachment->vkImage, attachment->vkImageMemory, 0));

        VkImageViewCreateInfo imageView{};
        imageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageView.format = format;
        imageView.subresourceRange = {};
        imageView.subresourceRange.aspectMask = aspectMask;
        imageView.subresourceRange.baseMipLevel = 0;
        imageView.subresourceRange.levelCount = 1;
        imageView.subresourceRange.baseArrayLayer = 0;
        imageView.subresourceRange.layerCount = 1;
        imageView.image = attachment->vkImage;
        VulkanUtils::CheckVKResult(vkCreateImageView(vulkanContext->device.device, &imageView, nullptr,
                                                     &attachment->vkImageView));
        return attachment;
    }


    void RenderTargetService::disposeResource(RenderTargetInstance *resource) {
        LOG_INFO("Disposing rendertarget instance");

        for (int i = 0; i < resource->attachments.size(); i++) {
            LOG_INFO("Disposing of attachment instance " + std::to_string(i));
            resource->attachments[i]->dispose();
        }
    }
} // Metal
