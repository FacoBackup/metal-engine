#ifndef FRAMEBUFFER_INSTANCE_H
#define FRAMEBUFFER_INSTANCE_H

#include "RuntimeResource.h"
#include <vector>
#include <glm/vec4.hpp>
#include "FrameBufferAttachment.h"

namespace Metal {
    struct FrameBufferInstance final : RuntimeResource {
        unsigned int bufferWidth{};
        unsigned int bufferHeight{};
        glm::vec4 clearColor;
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        std::vector<std::shared_ptr<FrameBufferAttachment> > attachments{};

        explicit FrameBufferInstance(const std::string &id) : RuntimeResource(id) {
        }

        ResourceType resourceType() override {
            return FRAMEBUFFER;
        }
    };
}

#endif
