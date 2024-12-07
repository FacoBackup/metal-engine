#include "RenderPassService.h"

#include "RenderPassInstance.h"
#include "../../../common/util/VulkanUtils.h"
#include "../framebuffer/FrameBufferAttachment.h"

namespace Metal {
    RenderPassInstance *RenderPassService::createPipeline(const uint32_t w, const uint32_t h) const {
        auto *pipeline = new RenderPassInstance();
        pipeline->bufferWidth = w;
        pipeline->bufferHeight = h;
        registerResource(pipeline);
        return pipeline;
    }

    void RenderPassService::createDepthAttachment(const RenderPassInstance *pipeline) const {
        VkFormat depthFormat = VulkanUtils::GetValidDepthFormat(vulkanContext.physDevice.physical_device);
        createAttachment("Depth attachment", depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, pipeline);
    }

    void RenderPassService::createAttachment(const char *name, VkFormat format,
                                           VkImageUsageFlagBits usage,
                                           const RenderPassInstance *pipeline) const {
        auto *attachment = new FrameBufferAttachment;
        attachment->name = name;
        pipeline->framebuffer->attachments.push_back(attachment);
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
    }

    void RenderPassService::createFrameBuffer(const RenderPassInstance *pipeline) const {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipeline->vkRenderPass;

        if (pipeline->framebuffer->attachments.empty()) {
            throw std::runtime_error("No framebuffer attachments available");
        }

        std::vector<VkImageView> attachments{};
        attachments.reserve(pipeline->framebuffer->attachments.size());
        for (const auto &attachment: pipeline->framebuffer->attachments) {
            attachments.push_back(attachment->vkImageView);
        }

        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = pipeline->bufferWidth;
        framebufferInfo.height = pipeline->bufferHeight;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(vulkanContext.device.device, &framebufferInfo, vulkanContext.instance.allocation_callbacks,
                            &pipeline->framebuffer->vkFramebuffer);
        registerResource(pipeline->framebuffer);
    }

    void RenderPassService::createRenderPass( RenderPassInstance *pipeline) const {
        const VkAttachmentReference *depthAttachmentRef = VK_NULL_HANDLE;
        for (uint32_t i = 0; i < static_cast<uint32_t>(pipeline->framebuffer->attachments.size()); i++) {
            // ATTACHMENT DESCRIPTION
            auto &attachmentDescription = pipeline->attachments.emplace_back();
            auto *fbAttachment = pipeline->framebuffer->attachments[i];
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

        // RENDER PASS DEPENDENCIES (SUB PASSES)
        pipeline->subPasses.emplace_back();
        pipeline->subPasses[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        pipeline->subPasses[0].dstSubpass = 0;
        pipeline->subPasses[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        pipeline->subPasses[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        pipeline->subPasses[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        pipeline->subPasses[0].dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        pipeline->subPasses[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        pipeline->subPasses.emplace_back();
        pipeline->subPasses[1].srcSubpass = 0;
        pipeline->subPasses[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        pipeline->subPasses[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        pipeline->subPasses[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        pipeline->subPasses[1].srcAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        pipeline->subPasses[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        pipeline->subPasses[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // SUB PASS DESCRIPTOR
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments = pipeline->colorReferences.data();
        subpass.colorAttachmentCount = static_cast<uint32_t>(pipeline->colorReferences.size());
        subpass.pDepthStencilAttachment = depthAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pAttachments = pipeline->attachments.data();
        renderPassInfo.attachmentCount = pipeline->attachments.size();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = pipeline->subPasses.size();
        renderPassInfo.pDependencies = pipeline->subPasses.data();
        VulkanUtils::CheckVKResult(vkCreateRenderPass(vulkanContext.device.device, &renderPassInfo, nullptr,
                                                      &pipeline->vkRenderPass));
    }
} // Metal
