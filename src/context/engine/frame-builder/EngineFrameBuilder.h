#ifndef METAL_ENGINE_ENGINEFRAMEBUILDER_H
#define METAL_ENGINE_ENGINEFRAMEBUILDER_H
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>

#include "structures/ResourceBuilder.h"
#include "../../../enum/PassType.h"
#include "../../../service/buffer/BufferInstance.h"

namespace Metal {
    class CommandBufferRecorder;
    struct FrameBufferInstance;
    struct DescriptorBinding;
    class ResourceBuilder;
    class EngineFrame;

    class EngineFrameBuilder final {
        std::string frameId;
        std::vector<std::shared_ptr<ResourceBuilder> > builders{};
        std::shared_ptr<ResourceBuilder> currentBuilder{};
        std::vector<PassType> passTypes{};

    public:
        explicit EngineFrameBuilder(std::string frameId = Util::uuidV4());
        EngineFrameBuilder &addFramebuffer(std::string id, unsigned w, unsigned h, glm::vec4 clearColor);

        EngineFrameBuilder &addFramebuffer(const std::string &id);

        EngineFrameBuilder &addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage,
                                     FrameBufferInstance *framebuffer);

        EngineFrameBuilder &addDepth();

        EngineFrameBuilder &addTexture(const std::string &id, unsigned w, unsigned h);

        EngineFrameBuilder &addTexture(const std::string &id);

        EngineFrameBuilder &addBuffer(const std::string &id, VkDeviceSize size,
                                      VkMemoryPropertyFlags properties, BufferType type);

        EngineFrameBuilder &addBuffer(const std::string &id);

        EngineFrameBuilder &addPass(PassType type);

        bool tryMatch(const std::string &id, ResourceType type);

        std::unique_ptr<EngineFrame> build();
    };
} // Metal

#endif //METAL_ENGINE_ENGINEFRAMEBUILDER_H
