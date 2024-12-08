#ifndef ABSTRACTCOREREPOSITORY_H
#define ABSTRACTCOREREPOSITORY_H
#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;
    class FrameBufferService;
    class ShaderService;
    class BufferService;
    class CommandPoolService;

    class AbstractCoreRepository : public AbstractRuntimeComponent {
    public:
        explicit AbstractCoreRepository(ApplicationContext &context);

    protected:
        VulkanContext &vulkanContext;
        FrameBufferService &framebufferService;
        ShaderService &shaderService;
        BufferService &bufferService;
        CommandPoolService &commandPoolService;
    };


} // Metal

#endif //ABSTRACTCOREREPOSITORY_H
