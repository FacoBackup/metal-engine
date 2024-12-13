#include "MeshService.h"
#include "../runtime/MeshInstance.h"
#include "../runtime/MeshData.h"
#include "../VulkanContext.h"

namespace Metal {
    MeshInstance *MeshService::createMesh(const MeshData &data) {
        auto *instance = new MeshInstance;
        registerResource(instance);

        instance->indexCount = data.indices.size();

        VkDeviceSize bufferSize = sizeof(data.data[0]) * data.data.size();
        instance->dataBuffer = vulkanContext.bufferService.createBuffer(bufferSize, &data.data);

        bufferSize = sizeof(data.indices[0]) * data.indices.size();
        instance->indexBuffer = vulkanContext.bufferService.createBuffer(bufferSize, &data.indices);

        return instance;
    }
} // Metal
