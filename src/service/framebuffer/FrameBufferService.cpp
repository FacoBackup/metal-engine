#include "FrameBufferService.h"

#include <iostream>
#include <glm/vec3.hpp>
#include <array>
#include "FrameBufferInstance.h"
#include "../../util/VulkanUtils.h"
#include "FrameBufferAttachment.h"

namespace Metal {
    void FrameBufferService::createSampler(bool linear, VkSampler vkImageSampler) {
        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.magFilter = linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        samplerCreateInfo.minFilter = linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        samplerCreateInfo.mipmapMode = linear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1;
        // TODO - ENABLE/DISABLE ANISOTROPY
        samplerCreateInfo.maxAnisotropy = 8;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        VulkanUtils::CheckVKResult(vkCreateSampler(vulkanContext.device.device, &samplerCreateInfo, nullptr,
                                                   &vkImageSampler));
    }

    FrameBufferInstance *FrameBufferService::createFrameBuffer(const unsigned w, const unsigned h, glm::vec4 clearColor) {
        auto *framebuffer = new FrameBufferInstance();
        framebuffer->bufferWidth = w;
        framebuffer->bufferHeight = h;
        framebuffer->clearColor = clearColor;
        registerResource(framebuffer);

        return framebuffer;
    }

    void FrameBufferService::createDepthAttachment(FrameBufferInstance *framebuffer) const {
        VkFormat depthFormat = VulkanUtils::GetValidDepthFormat(vulkanContext.physDevice.physical_device);
        const auto att = createAttachmentInternal("Depth attachment", depthFormat,
                                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, framebuffer);
        att->depth = true;
    }

    void FrameBufferService::createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                                              FrameBufferInstance *framebuffer) const {
        const auto att = createAttachmentInternal(name, format,
                                                  usage,
                                                  framebuffer);
        att->depth = false;
    }

    std::shared_ptr<FrameBufferAttachment> FrameBufferService::createAttachmentInternal(
        const char *name, VkFormat format,
        VkImageUsageFlagBits usage,
        FrameBufferInstance *framebuffer) const {
        std::shared_ptr<FrameBufferAttachment> attachment = std::make_shared<FrameBufferAttachment>();
        attachment->name = name;
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

        VulkanUtils::CheckVKResult(vkCreateImage(vulkanContext.device.device, &image, nullptr, &attachment->vkImage));

        VkMemoryAllocateInfo memAlloc{};
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(vulkanContext.device.device, attachment->vkImage, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = VulkanUtils::GetMemTypeIndex(vulkanContext.physicalDeviceMemoryProperties,
                                                                memReqs.memoryTypeBits,
                                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VulkanUtils::CheckVKResult(vkAllocateMemory(vulkanContext.device.device, &memAlloc, nullptr,
                                                    &attachment->vkImageMemory));
        VulkanUtils::CheckVKResult(
            vkBindImageMemory(vulkanContext.device.device, attachment->vkImage, attachment->vkImageMemory, 0));

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
        VulkanUtils::CheckVKResult(vkCreateImageView(vulkanContext.device.device, &imageView, nullptr,
                                                     &attachment->vkImageView));
        return attachment;
    }

    void FrameBufferService::createRenderPass(FrameBufferInstance *framebuffer) const {
        std::vector<VkAttachmentDescription> attachmentDescriptions{};
        std::vector<VkAttachmentReference> colorReferences{};
        VkAttachmentReference *depthRef = nullptr;
        for (unsigned int i = 0; i < framebuffer->attachments.size(); i++) {
            VkAttachmentDescription &attachmentDescription = attachmentDescriptions.emplace_back();
            const std::shared_ptr<FrameBufferAttachment> fbAttachment = framebuffer->attachments[i];
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.finalLayout = fbAttachment->depth
                                                    ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                                    : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            attachmentDescription.format = fbAttachment->format;

            if (fbAttachment->depth) {
                std::cout << "Depth Attachment " << i << std::endl;
                depthRef = new VkAttachmentReference{i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
            } else {
                std::cout << "Color Attachment " << i << std::endl;
                VkAttachmentReference &colorRef = colorReferences.emplace_back();
                colorRef.attachment = i;
                colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
        }

        std::array<VkSubpassDependency, 2> dependencies{};
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // SUB PASS DESCRIPTOR
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments = colorReferences.data();
        subpass.colorAttachmentCount = colorReferences.size();
        if (depthRef != nullptr) {
            subpass.pDepthStencilAttachment = depthRef;
        }

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.attachmentCount = attachmentDescriptions.size();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

        VulkanUtils::CheckVKResult(vkCreateRenderPass(vulkanContext.device.device,
                                                      &renderPassInfo,
                                                      nullptr,
                                                      &framebuffer->vkRenderPass));
        createVKFrameBuffer(framebuffer);
        delete depthRef;
    }

    void FrameBufferService::createVKFrameBuffer(FrameBufferInstance *framebuffer) const {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = framebuffer->vkRenderPass;

        if (framebuffer->attachments.empty()) {
            throw std::runtime_error("No framebuffer attachments available");
        }

        std::vector<VkImageView> attachments{};
        attachments.reserve(framebuffer->attachments.size());
        for (const auto &attachment: framebuffer->attachments) {
            attachments.push_back(attachment->vkImageView);
        }

        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = framebuffer->bufferWidth;
        framebufferInfo.height = framebuffer->bufferHeight;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(vulkanContext.device.device, &framebufferInfo, nullptr,
                            &framebuffer->vkFramebuffer);
    }
} // Metal
