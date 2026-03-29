#ifndef RENDERPASS_H
#define RENDERPASS_H
#include "../resource/RuntimeResource.h"
#include <array>
#include <vector>

#include "../../common/IInit.h"
#include "../resource/PipelineInstance.h"
#include "../../common/VulkanUtils.h"

#include "../render-graph/RGResourceManager.h"

namespace Metal {
    class FrameService;
    class AbstractPass;
    class ApplicationContext;
    struct RenderTargetInstance;

    struct RenderTargetAttachment;
    class CommandBufferRecorder final : public RuntimeResource, public IInit {
        std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> _commandBuffers{};
        VkViewport viewport{};
        VkRect2D scissor{};
        VkRenderingInfo renderingInfo{};
        std::vector<VkRenderingAttachmentInfo> colorAttachments{};
        VkRenderingAttachmentInfo depthAttachment{};
        std::vector<std::shared_ptr<RenderTargetAttachment>> attachments{};
        bool hasDepth = false;
        bool computePassMode;
        FrameService *frameService = nullptr;

        static void RecordCommandsInternal(
            const std::vector<std::unique_ptr<AbstractPass> > &passes,
            VkCommandBuffer vkCommandBuffer,
            const RGResourceManager& resources);

        void createRenderingInfo(const RenderTargetInstance *frameBuffer, bool clearBuffer);

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FrameService", &frameService}, {"VulkanContext", &vulkanContext}};
        }

        void onInitialize() override;

        explicit CommandBufferRecorder(std::string id, RenderTargetInstance *frameBuffer, bool clearBuffer = true);

        explicit CommandBufferRecorder(const std::string &id);

        ResourceType resourceType() override {
            return COMMAND_BUFFER_RECORDER;
        }

        void createCommandBuffer();

        void recordCommands(const std::vector<std::unique_ptr<AbstractPass> > &passes, const RGResourceManager& resources);
    };
} // Metal

#endif //RENDERPASS_H
