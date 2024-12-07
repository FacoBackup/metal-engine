#ifndef ABSTRACTRENDERPASS_H
#define ABSTRACTRENDERPASS_H
#include <vulkan/vulkan_core.h>

#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class AbstractRenderPass : public AbstractRuntimeComponent {
    protected:
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;

        virtual VkDescriptorSet createDescriptorSetInternal() { return nullptr; }

        virtual VkCommandBuffer createCommandBufferInternal() { return nullptr; }

        virtual VkRenderPass createRenderPassInternal() { return nullptr; }

        virtual void initializeFrameBuffers() {
        }

        virtual void updateCommandBufferInternal() {
        }

    public:
        explicit AbstractRenderPass(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void onSync() override;

        void onInitialize() override;

        VkRenderPass &getRenderPass() { return renderPass; }
    };
}

#endif
