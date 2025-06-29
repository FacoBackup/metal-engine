#ifndef RENDERPASSINSTANCE_H
#define RENDERPASSINSTANCE_H

#include "../VulkanContext.h"
#include "../../common/RuntimeResource.h"
#include <vector>
#include <iostream>
#include <glm/vec4.hpp>
#include "FrameBufferAttachment.h"
#include "../../MetalContextProvider.h"

namespace Metal {
    struct FrameBufferAttachment;
    struct ShaderModule;

    struct FrameBufferInstance final : RuntimeResource {
        unsigned int  bufferWidth{};
        unsigned int  bufferHeight{};
        glm::vec4 clearColor;
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        std::vector<std::shared_ptr<FrameBufferAttachment> > attachments{};

        void dispose() override {
            std::cout << "Disposing framebuffer instance" << std::endl;
            vkDestroyFramebuffer(SINGLETONS.vulkanContext.device.device, vkFramebuffer, nullptr);
            vkDestroyRenderPass(SINGLETONS.vulkanContext.device.device, vkRenderPass, nullptr);

            for (int i = 0; i < attachments.size(); i++) {
                std::cout << "Disposing of attachment instance " << i << std::endl;
                attachments[i]->dispose();
            }
        }

        ResourceType resourceType() override {
            return FRAMEBUFFER;
        }
    };
}

#endif
