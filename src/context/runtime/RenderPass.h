#ifndef RENDERPASS_H
#define RENDERPASS_H
#include <vector>
#include <vulkan/vulkan.h>

#include "PipelineInstance.h"
#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class AbstractRenderPass;

    class RenderPass final : public AbstractRuntimeComponent {
        std::vector<AbstractRenderPass *> renderPasses;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        VkRenderPassBeginInfo renderPassInfo{};

    public:
        explicit RenderPass(const FrameBufferInstance *frameBuffer, ApplicationContext &applicationContext);

        void onSync() override;

        void addRenderPass(AbstractRenderPass *renderPass);
    };
} // Metal

#endif //RENDERPASS_H
