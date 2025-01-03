#ifndef RENDERPASS_H
#define RENDERPASS_H
#include <vector>
#include "../../../service/pipeline/PipelineInstance.h"
#include "../../../util/VulkanUtils.h"

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

        static void RecordCommandsInternal(VkPipelineBindPoint binding, const std::vector<std::unique_ptr<AbstractPass> > &passes,
                                           VkCommandBuffer vkCommandBuffer);

        void createRenderPassInfo(FrameBufferInstance *frameBuffer, bool clearBuffer);

    public:
        explicit CommandBufferRecorder(FrameBufferInstance *frameBuffer, ApplicationContext &applicationContext,
                                       bool clearBuffer = true);

        explicit CommandBufferRecorder(ApplicationContext &applicationContext);

        void recordCommands(const std::vector<std::unique_ptr<AbstractPass> > &passes) const;
    };
} // Metal

#endif //RENDERPASS_H
