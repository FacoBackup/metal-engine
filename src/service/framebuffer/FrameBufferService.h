#ifndef RENDERPASSSERVICE_H
#define RENDERPASSSERVICE_H

#include <glm/vec4.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>

#include "../abstract/AbstractResourceService.h"
#include "FrameBufferInstance.h"

namespace Metal {
    struct FrameBufferAttachment;
    struct FrameBufferInstance;

    /**
     * Responsible for creating:
     * - Framebuffer attachments
     * - Command buffers
     * - Render passes
     * - Framebuffers
     */
    class FrameBufferService final : public AbstractResourceService<FrameBufferInstance> {
        std::shared_ptr<FrameBufferAttachment> createAttachmentInternal(const char *name, VkFormat format,
                                                                        VkImageUsageFlagBits usage,
                                                                        FrameBufferInstance *framebuffer) const;

        void createVKFrameBuffer(FrameBufferInstance *framebuffer) const;

    public:
        explicit FrameBufferService()
            : AbstractResourceService<FrameBufferInstance>() {
        }

        void createSampler(bool linear, VkSampler &vkImageSampler);

        FrameBufferInstance *createFrameBuffer(const std::string &id, unsigned int w, unsigned int h, glm::vec4 clearColor = glm::vec4(0.0f));

        void createDepthAttachment(FrameBufferInstance *framebuffer) const;

        void createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                              FrameBufferInstance *framebuffer) const;


        void createRenderPass(FrameBufferInstance *framebuffer) const;

        void disposeResource(FrameBufferInstance *resource) override;
    };
} // Metal

#endif //PIPELINESERVICE_H
