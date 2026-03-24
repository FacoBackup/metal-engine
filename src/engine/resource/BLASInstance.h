#ifndef BLASINSTANCE_H
#define BLASINSTANCE_H

#include <engine/resource/RuntimeResource.h>
#include <vulkan/vulkan.h>

namespace Metal {
    struct BufferInstance;

    struct BLASInstance final : RuntimeResource {
        VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
        BufferInstance* buffer = nullptr;
        BufferInstance* scratchBuffer = nullptr;
        BufferInstance* vertexData = nullptr;
        BufferInstance* indexData = nullptr;

        explicit BLASInstance(const std::string& id) : RuntimeResource(id) {}

        ResourceType resourceType() override {
            return BLAS_INSTANCE;
        }
    };
}

#endif //BLASINSTANCE_H
