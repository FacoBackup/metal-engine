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

    public:
        explicit EngineFrameBuilder(std::string frameId = Util::uuidV4());
        EngineFrameBuilder &addFramebuffer(std::string id, unsigned w, unsigned h, glm::vec4 clearColor);

        EngineFrameBuilder &addFramebuffer(const std::string &id);

        EngineFrameBuilder &addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage);

        EngineFrameBuilder &addDepth();

        EngineFrameBuilder &addTexture(const std::string &id, unsigned w, unsigned h, VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT);

        EngineFrameBuilder &addTexture(const std::string &id);

        EngineFrameBuilder &addBuffer(const std::string &id, VkDeviceSize size,
                                      VkMemoryPropertyFlags properties, BufferType type);

        EngineFrameBuilder &addBuffer(const std::string &id);

        EngineFrameBuilder &addCommandBuffer(const std::string &id, const std::string &framebufferId, bool clearBuffer = true);

        EngineFrameBuilder &addComputeCommandBuffer(const std::string &id);

        EngineFrameBuilder &addPass(PassType type, const std::string &commandBufferId);

        bool tryMatch(const std::string &id, ResourceType type);

        std::unique_ptr<EngineFrame> build();
    };
} // Metal

#endif //METAL_ENGINE_ENGINEFRAMEBUILDER_H
