#ifndef RENDERPASSSERVICE_H
#define RENDERPASSSERVICE_H

#include <memory>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>

#include "../../common/AbstractResourceService.h"


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
    class FrameBufferService final : public AbstractResourceService {
        std::shared_ptr<FrameBufferAttachment> createAttachmentInternal(const char *name, VkFormat format,
                                                                        VkImageUsageFlagBits usage,
                                                                        FrameBufferInstance *framebuffer) const;

        void createVKFrameBuffer(FrameBufferInstance *framebuffer) const;

    public:
        void createSampler(bool linear, VkSampler &vkImageSampler);

        FrameBufferInstance *createFrameBuffer(unsigned w, unsigned h, glm::vec4 clearColor = glm::vec4(0.0f));

        void createDepthAttachment(FrameBufferInstance *framebuffer) const;

        void createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                              FrameBufferInstance *framebuffer) const;


        void createRenderPass(FrameBufferInstance *framebuffer) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
