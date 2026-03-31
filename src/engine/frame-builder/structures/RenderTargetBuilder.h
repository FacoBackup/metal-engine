#ifndef METAL_ENGINE_RENDERTARGETBUILDER_H
#define METAL_ENGINE_RENDERTARGETBUILDER_H
#include <vector>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>
#include "ResourceBuilder.h"

namespace Metal {
    class RenderTargetService;
    struct RenderTargetInstance;

    struct RenderTargetAttachmentBuilder {
        VkFormat format;
        VkImageUsageFlags usage;
        RenderTargetInstance *renderTarget;
    };

    class RenderTargetBuilder final : public ResourceBuilder {
        const unsigned w;
        const unsigned h;
        glm::vec4 clearColor;
        std::vector<RenderTargetAttachmentBuilder> attachments;
        bool hasDepth = false;
        RenderTargetService *renderTargetService = nullptr;
    public:
        std::vector<Dependency> getDependencies() override {
            return {{"RenderTargetService", &renderTargetService}};
        }

        explicit RenderTargetBuilder(const std::string &id, const unsigned w, const unsigned h, glm::vec4 clearColor)
            : ResourceBuilder(id), clearColor(clearColor), w(w), h(h) {
        }

        ResourceType getType() override;

        void addColor( VkFormat format, VkImageUsageFlags usage,
                      RenderTargetInstance *renderTarget) {
            attachments.push_back({ format, usage, renderTarget});
        }

        void addDepth() {
            hasDepth = true;
        }

        RuntimeResource* build() override;
    };
} // Metal

#endif //METAL_ENGINE_RENDERTARGETBUILDER_H
