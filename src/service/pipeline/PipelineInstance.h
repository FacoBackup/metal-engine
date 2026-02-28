#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/abstract/RuntimeResource.h"
#include "../descriptor/DescriptorInstance.h"
#include <memory>

namespace Metal {
    struct FrameBufferInstance;
    struct ShaderModule;
    struct MeshInstance;
    struct DescriptorInstance;

    struct BufferInstance;

    struct PipelineInstance final : RuntimeResource {
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
        std::unique_ptr<DescriptorInstance> descriptor{};
        bool isCompute = false;
        bool isRayTracing = false;
        unsigned int pushConstantsSize = 0;

        explicit PipelineInstance(const std::string &id) : RuntimeResource(id) {
        }

        // Ray tracing SBT
        BufferInstance *raygenSBT = nullptr;
        BufferInstance *missSBT = nullptr;
        BufferInstance *hitSBT = nullptr;
        VkStridedDeviceAddressRegionKHR raygenRegion{};
        VkStridedDeviceAddressRegionKHR missRegion{};
        VkStridedDeviceAddressRegionKHR hitRegion{};
        VkStridedDeviceAddressRegionKHR callableRegion{};

        ResourceType resourceType() override {
            return PIPELINE;
        }
    };
}
#endif //PIPELINEINSTANCE_H
