#ifndef RENDERPASSSERVICE_H
#define RENDERPASSSERVICE_H

#include <glm/vec4.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>

#include "../../../common/AbstractResourceService.h"
#include "../resource/FrameBufferInstance.h"

namespace Metal {
    struct FrameBufferAttachment;
    struct FrameBufferInstance;

    class FrameBufferService final : public AbstractResourceService<FrameBufferInstance> {
        std::shared_ptr<FrameBufferAttachment> createAttachmentInternal(VkFormat format,
                                                                        VkImageUsageFlagBits usage,
                                                                        FrameBufferInstance *framebuffer) const;

        void createVKFrameBuffer(FrameBufferInstance *framebuffer) const;

    public:
        FrameBufferInstance *createFrameBuffer(const std::string &id, unsigned int w, unsigned int h,
                                               glm::vec4 clearColor = glm::vec4(0.0f));

        void createDepthAttachment(FrameBufferInstance *framebuffer) const;

        void createAttachment(VkFormat format, VkImageUsageFlagBits usage,
                              FrameBufferInstance *framebuffer) const;

        void createRenderPass(FrameBufferInstance *framebuffer) const;

        void disposeResource(FrameBufferInstance *resource) override;
    };
} // Metal

#endif //PIPELINESERVICE_H
