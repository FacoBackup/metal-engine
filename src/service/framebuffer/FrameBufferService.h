#ifndef RENDERPASSSERVICE_H
#define RENDERPASSSERVICE_H

#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>

#include "../abstract/AbstractResourceService.h"


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
        explicit FrameBufferService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        FrameBufferInstance *createFrameBuffer(uint32_t w, uint32_t h, glm::vec4 clearColor = glm::vec4(0.0f), bool linear = false);

        void createDepthAttachment(FrameBufferInstance *framebuffer) const;

        void createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                              FrameBufferInstance *framebuffer) const;


        void createRenderPass(FrameBufferInstance *framebuffer) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
