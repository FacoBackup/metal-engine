#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vulkan/vulkan_core.h>

#include "../AbstractResourceService.h"

namespace Metal {
    struct FrameBufferAttachment;
    struct RenderPassInstance;
    /**
     * Responsible for creating:
     * - Framebuffer attachments
     * - Command buffers
     * - Render passes
     * - Framebuffers
     */
    class RenderPassService final : public AbstractResourceService {
    public:
        explicit RenderPassService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        RenderPassInstance *createPipeline(uint32_t w, uint32_t h) const;

        void createDepthAttachment(const RenderPassInstance *pipeline) const;

        void createAttachment(const char *name, VkFormat format, VkImageUsageFlagBits usage,
                              const RenderPassInstance *pipeline) const;

        void createFrameBuffer(const RenderPassInstance *pipeline) const;

        void createRenderPass(RenderPassInstance *pipeline) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
