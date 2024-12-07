#ifndef FRAMEBUFFERSERVICE_H
#define FRAMEBUFFERSERVICE_H
#include "FramebufferInstance.h"
#include "../AbstractResourceService.h"


namespace Metal {
    class AbstractRenderPass;
    class TextureService;

    class FrameBufferService final : public AbstractResourceService {
        TextureService &textureService;

    public:
        explicit FrameBufferService(ApplicationContext &context);

        FramebufferInstance *createFrameBuffer(AbstractRenderPass &renderPass);
    };
} // Metal

#endif
