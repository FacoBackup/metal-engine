#ifndef RENDERPASSINSTANCE_H
#define RENDERPASSINSTANCE_H

#include "../VulkanContext.h"
#include "RuntimeResource.h"
#include <vector>

namespace Metal {
    struct FrameBufferAttachment;
    struct ShaderModule;

    struct FrameBufferInstance final : RuntimeResource {
        uint32_t bufferWidth{};
        uint32_t bufferHeight{};
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkAttachmentReference> colorReferences;
        VkAttachmentReference *depthRef = nullptr;
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        std::vector<std::shared_ptr<FrameBufferAttachment> > attachments{};

        void dispose(VulkanContext &context) override {
            delete depthRef;
            vkDestroyFramebuffer(context.device.device, vkFramebuffer, nullptr);
            vkDestroyRenderPass(context.device.device, vkRenderPass, nullptr);
        }
    };
}

#endif
