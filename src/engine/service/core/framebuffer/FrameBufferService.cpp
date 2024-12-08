#include "FrameBufferService.h"

#include "FrameBufferInstance.h"
#include "../../../../common/util/VulkanUtils.h"
#include "FrameBufferAttachment.h"

namespace Metal {
    FrameBufferInstance *FrameBufferService::createPipeline(const uint32_t w, const uint32_t h) const {
        auto *pipeline = new FrameBufferInstance();
        pipeline->bufferWidth = w;
        pipeline->bufferHeight = h;
        registerResource(pipeline);
        return pipeline;
    }

    void FrameBufferService::createDepthAttachment( FrameBufferInstance *pipeline) const {
        VkFormat depthFormat = VulkanUtils::GetValidDepthFormat(vulkanContext.physDevice.physical_device);
        FrameBufferAttachment *att = createAttachmentInternal("Depth attachment", depthFormat,
                                                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, pipeline);
        att->depth = true;
    }

    void FrameBufferService::createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                                              FrameBufferInstance *pipeline) const {
        FrameBufferAttachment *att = createAttachmentInternal(name, format, usage, pipeline);
        att->depth = false;
    }

    FrameBufferAttachment *FrameBufferService::createAttachmentInternal(const char *name, VkFormat format,
                                                                       VkImageUsageFlagBits usage,
                                                                       FrameBufferInstance *pipeline) const {
        auto *attachment = new FrameBufferAttachment;
        attachment->name = name;
        pipeline->attachments.push_back(attachment);
        registerResource(attachment);

        // Create the off-screen image
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent.width = pipeline->bufferWidth;
        imageInfo.extent.height = pipeline->bufferHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;

        // Allocate memory using VMA
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VmaAllocation offscreenImageAllocation;
        vmaCreateImage(vulkanContext.allocator, &imageInfo, &allocInfo, &attachment->vkImage,
                       &offscreenImageAllocation, nullptr);

        // Create an image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = attachment->vkImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(vulkanContext.device.device, &viewInfo, nullptr, &attachment->vkImageView);

        registerResource(attachment);
        return attachment;
    }

    void FrameBufferService::createFrameBuffer(FrameBufferInstance *pipeline) const {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipeline->vkRenderPass;

        if (pipeline->attachments.empty()) {
            throw std::runtime_error("No framebuffer attachments available");
        }

        std::vector<VkImageView> attachments{};
        attachments.reserve(pipeline->attachments.size());
        for (const auto &attachment: pipeline->attachments) {
            attachments.push_back(attachment->vkImageView);
        }

        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = pipeline->bufferWidth;
        framebufferInfo.height = pipeline->bufferHeight;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(vulkanContext.device.device, &framebufferInfo, vulkanContext.instance.allocation_callbacks,
                            &pipeline->vkFramebuffer);
    }

    void FrameBufferService::createRenderPass(FrameBufferInstance *pipeline) const {
        createFrameBuffer(pipeline);
        const VkAttachmentReference *depthAttachmentRef = VK_NULL_HANDLE;
        for (uint32_t i = 0; i < static_cast<uint32_t>(pipeline->attachments.size()); i++) {
            // ATTACHMENT DESCRIPTION
            auto &attachmentDescription = pipeline->attachmentDescriptions.emplace_back();
            auto *fbAttachment = pipeline->attachments[i];
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.finalLayout = fbAttachment->depth
                                                    ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                                    : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            attachmentDescription.format = fbAttachment->format;

            // COLOR REF
            auto &colorRef = pipeline->colorReferences.emplace_back();
            colorRef.attachment = i;
            colorRef.layout = fbAttachment->depth
                                  ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                  : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            if (fbAttachment->depth) {
                depthAttachmentRef = &colorRef;
            }
        }


        std::array<VkSubpassDependency, 2> dependencies{};
        // RENDER PASS DEPENDENCIES (SUB PASSES)
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
        subpass.pColorAttachments = pipeline->colorReferences.data();
        subpass.colorAttachmentCount = static_cast<uint32_t>(pipeline->colorReferences.size());
        if (depthAttachmentRef != VK_NULL_HANDLE) {
            subpass.pDepthStencilAttachment = depthAttachmentRef;
        }

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pAttachments = pipeline->attachmentDescriptions.data();
        renderPassInfo.attachmentCount = pipeline->attachmentDescriptions.size();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();
        auto vk_device = vulkanContext.device.device;
        VulkanUtils::CheckVKResult(vkCreateRenderPass(vk_device, &renderPassInfo,
                                                      vulkanContext.instance.allocation_callbacks,
                                                      &pipeline->vkRenderPass));
    }
} // Metal
