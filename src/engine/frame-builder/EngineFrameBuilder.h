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
#include "../render-graph/RGResourceHandle.h"
#include "../render-graph/RGResourceDescription.h"

namespace Metal {
    class EngineContext;
    class AbstractPass;
    class CommandBufferRecorder;
    struct RenderTargetInstance;
    struct DescriptorBinding;
    class ResourceBuilder;
    class VulkanContext;
    class EngineFrame;

    /**
     * @struct PassData
     * @brief Holds information about a render pass and its associated command buffer.
     */
    struct PassData {
        std::unique_ptr<AbstractPass> pass; ///< The render pass implementation.
        std::string commandBufferId;        ///< ID of the command buffer to record into.
    };

    /**
     * @class EngineFrameBuilder
     * @brief Fluent builder for constructing an EngineFrame.
     *
     * This builder allows defining the resources (framebuffers, textures, buffers)
     * and the sequence of render passes that constitute a single engine frame.
     */
    class EngineFrameBuilder final : public IContextMember {
        std::string frameId;
        std::vector<std::shared_ptr<ResourceBuilder> > builders{};
        std::vector<PassData> passes{};
        std::shared_ptr<ResourceBuilder> currentBuilder{};
        EngineContext *engineContext = nullptr;
        VulkanContext *vulkanContext = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EngineContext", &engineContext},
                {"VulkanContext", &vulkanContext}
            };
        }

        /**
         * @brief Constructs a frame builder with a unique ID.
         */
        explicit EngineFrameBuilder(std::string frameId = Util::uuidV4());

        /**
         * @brief Adds a new rendertarget resource to the frame.
         */
        EngineFrameBuilder& addRenderTarget(std::string id, unsigned w, unsigned h, glm::vec4 clearColor, RGResourceHandle* outHandle = nullptr);

        /**
         * @brief References an existing rendertarget by ID.
         */
        EngineFrameBuilder& addRenderTarget(const std::string &id, RGResourceHandle* outHandle = nullptr);

        /**
         * @brief Adds a color attachment to the current rendertarget being built.
         */
        EngineFrameBuilder &addColor(VkFormat format, VkImageUsageFlags usage);

        /**
         * @brief Adds a depth attachment to the current rendertarget being built.
         */
        EngineFrameBuilder &addDepth();

        /**
         * @brief Adds a standalone texture resource to the frame.
         */
        EngineFrameBuilder& addTexture(const std::string &id, unsigned w, unsigned h,
                                       VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT, RGResourceHandle* outHandle = nullptr);

        /**
         * @brief References an existing texture by ID.
         */
        EngineFrameBuilder& addTexture(const std::string &id, RGResourceHandle* outHandle = nullptr);

        /**
         * @brief Adds a buffer resource (Uniform, Storage, etc.) to the frame.
         */
        EngineFrameBuilder& addBuffer(const std::string &id, VkDeviceSize size,
                                      VkMemoryPropertyFlags properties, BufferType type, RGResourceHandle* outHandle = nullptr);

        /**
         * @brief References an existing buffer by ID.
         */
        EngineFrameBuilder& addBuffer(const std::string &id, RGResourceHandle* outHandle = nullptr);

        /**
         * @brief Adds a graphics command buffer associated with a rendertarget.
         */
        EngineFrameBuilder &addCommandBuffer(const std::string &id, const std::string &renderTargetId,
                                             bool clearBuffer = true);

        /**
         * @brief Stores the current resource builder into the internal list.
         */
        void storeBuilder();

        /**
         * @brief Adds a compute command buffer, optionally supporting ray tracing.
         */
        EngineFrameBuilder &addComputeCommandBuffer(const std::string &id, bool requiresRayTracing = false);

        /**
         * @brief Adds a render pass to be executed in this frame.
         */
        EngineFrameBuilder &addPass(std::unique_ptr<AbstractPass> pass, const std::string &commandBufferId);

        /**
         * @brief Checks if a resource with the given ID and type exists in the builder.
         */
        bool tryMatch(const std::string &id, ResourceType type);

        /**
         * @brief Finalizes building and registers the frame with the EngineContext.
         */
        void build();
    };
} // Metal

#endif //METAL_ENGINE_ENGINEFRAMEBUILDER_H
