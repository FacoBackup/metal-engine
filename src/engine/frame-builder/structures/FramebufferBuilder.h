#ifndef METAL_ENGINE_FRAMEBUFFERBUILDER_H
#define METAL_ENGINE_FRAMEBUFFERBUILDER_H
#include <vector>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>
#include "ResourceBuilder.h"

namespace Metal {
    class FrameBufferService;
    struct FrameBufferInstance;

    struct FramebufferAttachmentBuilder {
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
        FrameBufferService *frameBufferService = nullptr;
    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FrameBufferService", &frameBufferService}};
        }

        explicit FramebufferBuilder(const std::string &id, const unsigned w, const unsigned h, glm::vec4 clearColor)
            : ResourceBuilder(id), clearColor(clearColor), w(w), h(h) {
        }

        ResourceType getType() override;

        void addColor( VkFormat format, VkImageUsageFlagBits usage,
                      FrameBufferInstance *framebuffer) {
            attachments.push_back({ format, usage, framebuffer});
        }

        void addDepth() {
            hasDepth = true;
        }

        RuntimeResource* build() override;
    };
} // Metal

#endif //METAL_ENGINE_FRAMEBUFFERBUILDER_H
