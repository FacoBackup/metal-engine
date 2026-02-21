#ifndef RENDERPASSINSTANCE_H
#define RENDERPASSINSTANCE_H

#include "../../repository/abstract/RuntimeResource.h"
#include <vector>
#include <glm/vec4.hpp>
#include "FrameBufferAttachment.h"

namespace Metal {
    struct ShaderModule;

    struct FrameBufferInstance final : RuntimeResource {
        unsigned int bufferWidth{};
        unsigned int bufferHeight{};
        glm::vec4 clearColor;
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        std::vector<std::shared_ptr<FrameBufferAttachment> > attachments{};

        void dispose() override;

        ResourceType resourceType() override {
            return FRAMEBUFFER;
        }
    };
}

#endif
