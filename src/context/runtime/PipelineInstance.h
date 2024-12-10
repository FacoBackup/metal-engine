#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../VulkanContext.h"

namespace Metal {
    struct FrameBufferInstance;
    struct ShaderModuleInstance;

    struct PipelineInstance final : RuntimeResource {
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
        FrameBufferInstance *frameBuffer = nullptr;
        ShaderModuleInstance *vertexShader = nullptr;
        ShaderModuleInstance *fragmentShader = nullptr;
        CommandBufferInstance *commandBuffer = nullptr;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE; // TODO - Pointer to a list of DescriptorInstance; allow multiple descriptor sets for a given pipeline

        void dispose(VulkanContext &context) override {
            vkDestroyPipelineLayout(context.device.device, vkPipelineLayout, nullptr);
            vkDestroyPipeline(context.device.device, vkPipeline, nullptr);
        }
    };
}
#endif //PIPELINEINSTANCE_H
