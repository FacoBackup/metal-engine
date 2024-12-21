#ifndef RENDERPASS_H
#define RENDERPASS_H
#include <vector>
#include <vulkan/vulkan.h>

#include "PipelineInstance.h"

namespace Metal {
    class AbstractRenderPass;
    class ApplicationContext;

    class RenderPass final {
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        VkRenderPassBeginInfo renderPassInfo{};
        ApplicationContext &context;
        VkViewport viewport{};
        VkRect2D scissor{};
    public:
        explicit RenderPass(const FrameBufferInstance *frameBuffer, ApplicationContext &applicationContext);

        void recordCommands(const std::vector<std::unique_ptr<AbstractRenderPass> > &renderPasses) const;
    };
} // Metal

#endif //RENDERPASS_H
