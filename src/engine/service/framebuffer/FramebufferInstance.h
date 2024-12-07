#ifndef FRAMEBUFFERINSTANCE_H
#define FRAMEBUFFERINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/resource/RuntimeResource.h"

namespace Metal {
    struct FrameBufferAttachment;

    struct FramebufferInstance final : RuntimeResource {
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        std::vector<FrameBufferAttachment *> attachments{};
    };
} // Metal

#endif //FRAMEBUFFERINSTANCE_H
