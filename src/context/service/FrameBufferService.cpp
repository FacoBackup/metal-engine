#include "FrameBufferService.h"

#include "../runtime/FrameBufferInstance.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/FrameBufferAttachment.h"

namespace Metal {
    FrameBufferInstance *FrameBufferService::createFrameBuffer(const uint32_t w, const uint32_t h) const {
        auto *framebuffer = new FrameBufferInstance();
        framebuffer->bufferWidth = w;
        framebuffer->bufferHeight = h;
        registerResource(framebuffer);
        return framebuffer;
    }

    void FrameBufferService::createDepthAttachment(FrameBufferInstance *framebuffer) const {
        VkFormat depthFormat = VulkanUtils::GetValidDepthFormat(vulkanContext.physDevice.physical_device);
        FrameBufferAttachment *att = createAttachmentInternal("Depth attachment", depthFormat,
                                                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, framebuffer);
        att->depth = true;
    }

    void FrameBufferService::createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                                              FrameBufferInstance *pipeline) const {
        FrameBufferAttachment *att = createAttachmentInternal(name, format, usage, pipeline);
        att->depth = false;
    }

    FrameBufferAttachment *FrameBufferService::createAttachmentInternal(const char *name, VkFormat format,
                                                                        VkImageUsageFlagBits usage,
                                                                        FrameBufferInstance *framebuffer) const {
        auto *attachment = new FrameBufferAttachment;
        attachment->name = name;
        framebuffer->attachments.push_back(attachment);
        attachment->format = format;
        registerResource(attachment);


        VkImageAspectFlags aspectMask = 0;
        attachment->format = format;

        if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
            aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
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

        VulkanUtils::CheckVKResult(vkCreateImage(vulkanContext.device.device, &image, nullptr, &attachment->vkImage));

        VkMemoryAllocateInfo memAlloc{};
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(vulkanContext.device.device, attachment->vkImage, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = VulkanUtils::GetMemTypeIndex(vulkanContext.physicalDeviceMemoryProperties,
                                                                memReqs.memoryTypeBits,
                                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VulkanUtils::CheckVKResult(vkAllocateMemory(vulkanContext.device.device, &memAlloc, nullptr, &attachment->mem));
        VulkanUtils::CheckVKResult(
            vkBindImageMemory(vulkanContext.device.device, attachment->vkImage, attachment->mem, 0));

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


        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
        samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1;
        samplerCreateInfo.maxAnisotropy = 8;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        VulkanUtils::CheckVKResult(vkCreateSampler(vulkanContext.device.device, &samplerCreateInfo, nullptr, &attachment->vkImageSampler));

        return attachment;
    }

    void FrameBufferService::createRenderPass(FrameBufferInstance *framebuffer) const {
        for (uint32_t i = 0; i < static_cast<uint32_t>(framebuffer->attachments.size()); i++) {
            // ATTACHMENT DESCRIPTION
            VkAttachmentDescription attachmentDescription{};
            const auto *fbAttachment = framebuffer->attachments[i];
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.finalLayout = fbAttachment->depth
                                                    ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                                    : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescription.format = fbAttachment->format;
            framebuffer->attachmentDescriptions.push_back(attachmentDescription);

            if (fbAttachment->depth) {
                framebuffer->depthRef = new VkAttachmentReference{i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
            } else {
                VkAttachmentReference colorRef{};
                colorRef.attachment = i;
                colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                framebuffer->colorReferences.push_back(colorRef);
            }
        }

        std::array<VkSubpassDependency, 2> dependencies{};
        // Prepare for rendering in the first subpass by synchronizing external operations and ensuring the attachment is ready for use.
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // Ensure the subpass completes all rendering operations before handing off to external operations.
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // SUB PASS DESCRIPTOR
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments = framebuffer->colorReferences.data();
        subpass.colorAttachmentCount = static_cast<uint32_t>(framebuffer->colorReferences.size());
        if (framebuffer->depthRef != nullptr) {
            subpass.pDepthStencilAttachment = framebuffer->depthRef;
        }

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pAttachments = framebuffer->attachmentDescriptions.data();
        renderPassInfo.attachmentCount = static_cast<uint32_t>(framebuffer->attachmentDescriptions.size());
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        VulkanUtils::CheckVKResult(vkCreateRenderPass(vulkanContext.device.device,
                                                      &renderPassInfo,
                                                      nullptr,
                                                      &framebuffer->vkRenderPass));
        createVKFrameBuffer(framebuffer);
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

        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = framebuffer->bufferWidth;
        framebufferInfo.height = framebuffer->bufferHeight;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(vulkanContext.device.device, &framebufferInfo, nullptr,
                            &framebuffer->vkFramebuffer);
    }
} // Metal
