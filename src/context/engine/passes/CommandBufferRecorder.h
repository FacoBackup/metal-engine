#ifndef RENDERPASS_H
#define RENDERPASS_H
#include <vector>
#include "../../../service/pipeline/PipelineInstance.h"
#include "../../../util/VulkanUtils.h"
#include <array>
namespace Metal {
    class AbstractPass;
    class ApplicationContext;
    struct FrameBufferInstance;

    class CommandBufferRecorder final {
        std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> _commandBuffers{};
        ApplicationContext &context;
        VkViewport viewport{};
        VkRect2D scissor{};
        VkRenderPassBeginInfo renderPassInfo{};
        std::vector<VkClearValue> clearColors{};
        bool computePassMode;

        static void RecordCommandsInternal(
            const std::vector<AbstractPass *> &passes,
            VkCommandBuffer vkCommandBuffer);

        void createRenderPassInfo(const FrameBufferInstance *frameBuffer, bool clearBuffer);

    public:
        explicit CommandBufferRecorder(FrameBufferInstance *frameBuffer, ApplicationContext &applicationContext,
                                       bool clearBuffer = true);

        void createCommandBuffer();

        explicit CommandBufferRecorder(ApplicationContext &applicationContext);

        void recordCommands(const std::vector<AbstractPass *> &passes) const;
    };
} // Metal

#endif //RENDERPASS_H
