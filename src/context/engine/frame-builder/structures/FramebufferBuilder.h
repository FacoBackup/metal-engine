#ifndef METAL_ENGINE_FRAMEBUFFERBUILDER_H
#define METAL_ENGINE_FRAMEBUFFERBUILDER_H
#include <vector>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>
#include "ResourceBuilder.h"

namespace Metal {
    struct FrameBufferInstance;

    struct FramebufferAttachmentBuilder {
        std::string id;
        VkFormat format;
        VkImageUsageFlagBits usage;
        FrameBufferInstance *framebuffer;
    };

    class FramebufferBuilder final : public ResourceBuilder {
        const unsigned w;
        const unsigned h;
        glm::vec4 clearColor;
        std::vector<FramebufferAttachmentBuilder> attachments;
        bool hasDepth = false;

    public:
        explicit FramebufferBuilder(const std::string &id, const unsigned w, const unsigned h, glm::vec4 clearColor)
            : ResourceBuilder(id), clearColor(clearColor), w(w), h(h) {
        }

        ResourceType getType() override;

        void addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage,
                      FrameBufferInstance *framebuffer) {
            attachments.push_back({std::move(id), format, usage, framebuffer});
        }

        void addDepth() {
            hasDepth = true;
        }

        RuntimeResource* build() override;
    };
} // Metal

#endif //METAL_ENGINE_FRAMEBUFFERBUILDER_H
