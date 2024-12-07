#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include "../../repository/resource/RuntimeResource.h"
#include "../framebuffer/FramebufferInstance.h"
#include "../shader/ShaderModuleInstance.h"

namespace Metal {
    struct RenderPassInstance final : RuntimeResource {
        uint32_t bufferWidth{};
        uint32_t bufferHeight{};
        VkRenderPass vkRenderPass;
        std::vector<VkAttachmentDescription> attachments{};
        std::vector<VkSubpassDependency> subPasses{};
        std::vector<VkAttachmentReference> colorReferences{};
        FramebufferInstance *framebuffer = new FramebufferInstance();

        void dispose(VulkanContext &context) override {
        }
    };
}

#endif
