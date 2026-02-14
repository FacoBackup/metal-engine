#ifndef RENDERPASSINSTANCE_H
#define RENDERPASSINSTANCE_H

#include "../../context/vulkan/VulkanContext.h"
#include "../../repository/abstract/RuntimeResource.h"
#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"
#include <vector>
#include <iostream>
#include <glm/vec4.hpp>
#include "FrameBufferAttachment.h"

namespace Metal {
    struct FrameBufferAttachment;
    struct ShaderModule;

    struct FrameBufferInstance final : RuntimeResource {
        unsigned int bufferWidth{};
        unsigned int bufferHeight{};
        glm::vec4 clearColor;
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        std::vector<std::shared_ptr<FrameBufferAttachment> > attachments{};

        void dispose(VulkanContext &context) override {
            LOG_INFO_S("Disposing framebuffer instance");
            vkDestroyFramebuffer(context.device.device, vkFramebuffer, nullptr);
            vkDestroyRenderPass(context.device.device, vkRenderPass, nullptr);

            for (int i = 0; i < attachments.size(); i++) {
                LOG_INFO_S("Disposing of attachment instance " + std::to_string(i));
                attachments[i]->dispose(context);
            }
        }

        ResourceType resourceType() override {
            return FRAMEBUFFER;
        }
    };
}

#endif
