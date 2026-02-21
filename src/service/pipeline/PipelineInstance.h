#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/abstract/RuntimeResource.h"

namespace Metal {
    struct FrameBufferInstance;
    struct ShaderModule;
    struct MeshInstance;
    struct DescriptorInstance;

    struct BufferInstance;

    struct PipelineInstance final : RuntimeResource {
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets{};
        bool isCompute = false;
        bool isRayTracing = false;
        unsigned int pushConstantsSize = 0;

        // Ray tracing SBT
        std::shared_ptr<BufferInstance> raygenSBT = nullptr;
        std::shared_ptr<BufferInstance> missSBT = nullptr;
        std::shared_ptr<BufferInstance> hitSBT = nullptr;
        VkStridedDeviceAddressRegionKHR raygenRegion{};
        VkStridedDeviceAddressRegionKHR missRegion{};
        VkStridedDeviceAddressRegionKHR hitRegion{};
        VkStridedDeviceAddressRegionKHR callableRegion{};

        void dispose() override;

        ResourceType resourceType() override {
            return PIPELINE;
        }
    };
}
#endif //PIPELINEINSTANCE_H
