#ifndef ABSTRACTCOREREPOSITORY_H
#define ABSTRACTCOREREPOSITORY_H
#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;
    class RenderPassService;
    class ShaderService;

    class AbstractCoreRepository : public AbstractRuntimeComponent {
    public:
        explicit AbstractCoreRepository(ApplicationContext &context);

    protected:
        VulkanContext &vulkanContext;
        RenderPassService &renderPassService;
        ShaderService &shaderService;
    };


} // Metal

#endif //ABSTRACTCOREREPOSITORY_H
