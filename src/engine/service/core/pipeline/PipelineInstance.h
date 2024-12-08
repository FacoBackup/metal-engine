#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../../repository/resource/RuntimeResource.h"

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
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        void dispose(VulkanContext &context) override {
            // TODO
        }
    };
}
#endif //PIPELINEINSTANCE_H
