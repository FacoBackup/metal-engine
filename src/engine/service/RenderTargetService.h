#ifndef RENDERTARGETSERVICE_H
#define RENDERTARGETSERVICE_H

#include <glm/vec4.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>

#include "../../common/AbstractResourceService.h"
#include "../resource/RenderTargetInstance.h"

namespace Metal {
    struct RenderTargetAttachment;
    struct RenderTargetInstance;

    class VulkanContext;

    class RenderTargetService final : public AbstractResourceService<RenderTargetInstance> {
        VulkanContext *vulkanContext = nullptr;

        std::shared_ptr<RenderTargetAttachment> createAttachmentInternal(VkFormat format,
                                                                        VkImageUsageFlags usage,
                                                                        RenderTargetInstance *framebuffer) const;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"VulkanContext", &vulkanContext}};
        }

        RenderTargetInstance *createRenderTarget(const std::string &id, unsigned int w, unsigned int h,
                                               glm::vec4 clearColor = glm::vec4(0.0f));

        void createDepthAttachment(RenderTargetInstance *framebuffer) const;

        void createAttachment(VkFormat format, VkImageUsageFlags usage,
                              RenderTargetInstance *framebuffer) const;

        void disposeResource(RenderTargetInstance *resource) override;
    };
} // Metal

#endif // RENDERTARGETSERVICE_H
