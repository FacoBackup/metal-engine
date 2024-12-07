#ifndef FRAMEBUFFERINSTANCE_H
#define FRAMEBUFFERINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/resource/RuntimeResource.h"

namespace Metal {
    struct  FramebufferInstance final : RuntimeResource{
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
    };
} // Metal

#endif //FRAMEBUFFERINSTANCE_H
