#ifndef METAL_ENGINE_ENGINEFRAMEBUILDER_H
#define METAL_ENGINE_ENGINEFRAMEBUILDER_H
#include <memory>
#include <string>
#include <vector>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>

#include "../../common/IContextMember.h"
#include "structures/ResourceBuilder.h"
#include "../resource/BufferInstance.h"

namespace Metal {
    class EngineContext;
    class AbstractPass;
    class CommandBufferRecorder;
    struct FrameBufferInstance;
    struct DescriptorBinding;
    class ResourceBuilder;
    class EngineFrame;

    struct PassData {
        std::unique_ptr<AbstractPass> pass;
        std::string commandBufferId;
    };

    class EngineFrameBuilder final : public IContextMember {
        std::string frameId;
        std::vector<std::shared_ptr<ResourceBuilder> > builders{};
        std::vector<PassData> passes{};
        std::shared_ptr<ResourceBuilder> currentBuilder{};
        EngineContext *engineContext = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"EngineContext", engineContext}};
        }

        explicit EngineFrameBuilder(std::string frameId = Util::uuidV4());

        EngineFrameBuilder &addFramebuffer(std::string id, unsigned w, unsigned h, glm::vec4 clearColor);

        EngineFrameBuilder &addFramebuffer(const std::string &id);

        EngineFrameBuilder &addColor(VkFormat format, VkImageUsageFlagBits usage);

        EngineFrameBuilder &addDepth();

        EngineFrameBuilder &addTexture(const std::string &id, unsigned w, unsigned h,
                                       VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT);

        EngineFrameBuilder &addTexture(const std::string &id);

        EngineFrameBuilder &addBuffer(const std::string &id, VkDeviceSize size,
                                      VkMemoryPropertyFlags properties, BufferType type);

        EngineFrameBuilder &addBuffer(const std::string &id);

        EngineFrameBuilder &addCommandBuffer(const std::string &id, const std::string &framebufferId,
                                             bool clearBuffer = true);

        void storeBuilder();

        EngineFrameBuilder &addComputeCommandBuffer(const std::string &id);

        EngineFrameBuilder &addPass(std::unique_ptr<AbstractPass> pass, const std::string &commandBufferId);

        bool tryMatch(const std::string &id, ResourceType type);

        void build();
    };
} // Metal

#endif //METAL_ENGINE_ENGINEFRAMEBUILDER_H
