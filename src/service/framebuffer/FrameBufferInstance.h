#ifndef RENDERPASSINSTANCE_H
#define RENDERPASSINSTANCE_H

#include "../../context/vulkan/VulkanContext.h"
#include "../../repository/abstract/RuntimeResource.h"
#include <vector>
#include <iostream>
#include <glm/vec4.hpp>
#include "FrameBufferAttachment.h"

namespace Metal {
    struct FrameBufferAttachment;
    struct ShaderModule;

    struct FrameBufferInstance final : RuntimeResource {
        uint32_t bufferWidth{};
        uint32_t bufferHeight{};
        glm::vec4 clearColor;
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        std::vector<std::shared_ptr<FrameBufferAttachment> > attachments{};

        void dispose(VulkanContext &context) override {
            std::cout << "Disposing framebuffer instance" << std::endl;
            vkDestroyFramebuffer(context.device.device, vkFramebuffer, nullptr);
            vkDestroyRenderPass(context.device.device, vkRenderPass, nullptr);

            for (int i = 0; i < attachments.size(); i++) {
                std::cout << "Disposing of attachment instance " << i << std::endl;
                attachments[i]->dispose(context);
            }
        }

        ResourceType resourceType() override {
            return FRAMEBUFFER;
        }
    };
}

#endif
