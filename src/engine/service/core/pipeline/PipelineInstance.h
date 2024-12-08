#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../../repository/resource/RuntimeResource.h"

namespace Metal {
    struct FrameBufferInstance;
    struct ShaderModuleInstance;

    struct PipelineInstance final : RuntimeResource {
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
        FrameBufferInstance *renderPass = nullptr;
        ShaderModuleInstance *vertexShader = nullptr;
        ShaderModuleInstance *fragmentShader = nullptr;

        void dispose(VulkanContext &context) override {
            // TODO
        }
    };
}
#endif //PIPELINEINSTANCE_H
