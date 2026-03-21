#ifndef RENDERPASS_H
#define RENDERPASS_H
#include "../resource/RuntimeResource.h"
#include <array>
#include <vector>

#include "../../common/IInit.h"
#include "../resource/PipelineInstance.h"
#include "../../common/VulkanUtils.h"

namespace Metal {
    class FrameService;
    class AbstractPass;
    class ApplicationContext;
    struct FrameBufferInstance;

    class CommandBufferRecorder final : public RuntimeResource, public IInit {
        std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> _commandBuffers{};
        VkViewport viewport{};
        VkRect2D scissor{};
        VkRenderPassBeginInfo renderPassInfo{};
        std::vector<VkClearValue> clearColors{};
        bool computePassMode;
        FrameService *frameService = nullptr;

        static void RecordCommandsInternal(
            const std::vector<std::unique_ptr<AbstractPass> > &passes,
            VkCommandBuffer vkCommandBuffer);

        void createRenderPassInfo(const FrameBufferInstance *frameBuffer, bool clearBuffer);

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FrameService", &frameService}, {"VulkanContext", &vulkanContext}};
        }

        void onInitialize() override;

        explicit CommandBufferRecorder(std::string id, FrameBufferInstance *frameBuffer, bool clearBuffer = true);

        explicit CommandBufferRecorder(const std::string &id);

        ResourceType resourceType() override {
            return COMMAND_BUFFER_RECORDER;
        }

        void createCommandBuffer();

        void recordCommands(const std::vector<std::unique_ptr<AbstractPass> > &passes) const;
    };
} // Metal

#endif //RENDERPASS_H
