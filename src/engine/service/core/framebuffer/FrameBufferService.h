#ifndef RENDERPASSSERVICE_H
#define RENDERPASSSERVICE_H

#include <vulkan/vulkan_core.h>

#include "../../AbstractResourceService.h"

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
        FrameBufferAttachment *createAttachmentInternal(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                                                        FrameBufferInstance *framebuffer) const;

        void createVKFrameBuffer(FrameBufferInstance *framebuffer) const;

    public:
        explicit FrameBufferService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] FrameBufferInstance *createFrameBuffer(uint32_t w, uint32_t h) const;

        void createDepthAttachment(FrameBufferInstance *framebuffer) const;

        void createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                              FrameBufferInstance *pipeline) const;


        void createRenderPass(FrameBufferInstance *framebuffer) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
