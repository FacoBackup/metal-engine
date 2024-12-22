#ifndef RENDERPASS_H
#define RENDERPASS_H
#include <vector>
#include "PipelineInstance.h"
#include "../../common/util/VulkanUtils.h"

namespace Metal {
    class AbstractRenderPass;
    class ApplicationContext;
    struct FrameBufferInstance;

    class CommandBufferRecorder final {
        std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> _commandBuffers{};
        ApplicationContext &context;
        VkViewport viewport{};
        VkRect2D scissor{};
        VkRenderPassBeginInfo renderPassInfo{};

    public:
        void createRenderPassInfo(FrameBufferInstance *frameBuffer, bool clearBuffer);

        explicit CommandBufferRecorder(FrameBufferInstance *frameBuffer, ApplicationContext &applicationContext,
                                       bool clearBuffer = true);

        void recordCommands(const std::vector<std::unique_ptr<AbstractRenderPass> > &renderPasses) const;
    };
} // Metal

#endif //RENDERPASS_H
