#ifndef METAL_ENGINE_ENGINEFRAMEBUILDER_H
#define METAL_ENGINE_ENGINEFRAMEBUILDER_H
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Metal {
    struct FrameBufferInstance;
    class ResourceBuilder;

    class EngineFrameBuilder final {
        std::vector<std::shared_ptr<ResourceBuilder> > builders{};
        std::shared_ptr<ResourceBuilder> currentBuilder{};

    public:
        EngineFrameBuilder &addFramebuffer(std::string id);

        EngineFrameBuilder &addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage,
                                     FrameBufferInstance *framebuffer);
    };
} // Metal

#endif //METAL_ENGINE_ENGINEFRAMEBUILDER_H
