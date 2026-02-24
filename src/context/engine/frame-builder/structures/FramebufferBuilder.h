//
// Created by GustavoRoque on 24/02/2026.
//

#ifndef METAL_ENGINE_FRAMEBUFFERBUILDER_H
#define METAL_ENGINE_FRAMEBUFFERBUILDER_H
#include <vulkan/vulkan_core.h>

#include "ResourceBuilder.h"

namespace Metal {
    struct FrameBufferInstance;

    class FramebufferBuilder final : public ResourceBuilder {
    public:
        explicit FramebufferBuilder(const std::string &id)
            : ResourceBuilder(id) {
        }

        void addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage,
                      FrameBufferInstance *framebuffer);
    };
} // Metal

#endif //METAL_ENGINE_FRAMEBUFFERBUILDER_H
