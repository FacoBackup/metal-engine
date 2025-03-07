#ifndef ABSTRACTCOREREPOSITORY_H
#define ABSTRACTCOREREPOSITORY_H
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;
    class FrameBufferService;
    class BufferService;
    class PipelineService;

    class AbstractCoreRepository : public AbstractRuntimeComponent {
    public:
        explicit AbstractCoreRepository(ApplicationContext &context);

    protected:
        VulkanContext &vulkanContext;
        FrameBufferService &framebufferService;
        BufferService &bufferService;
        PipelineService &pipelineService;
    };


} // Metal

#endif //ABSTRACTCOREREPOSITORY_H
