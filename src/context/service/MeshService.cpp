#include "MeshService.h"
#include "../runtime/MeshInstance.h"
#include "../runtime/MeshData.h"
#include "../VulkanContext.h"

namespace Metal {
    MeshInstance *MeshService::createMesh(const MeshData &data) {
        auto *instance = new MeshInstance;
        registerResource(instance);

        VkDeviceSize bufferSize = sizeof(data.vertices[0]) * data.vertices.size();
        instance->vertexBuffer = vulkanContext.bufferService.createBuffer(bufferSize, &data.vertices);

        bufferSize = sizeof(data.indices[0]) * data.indices.size();
        instance->indexBuffer = vulkanContext.bufferService.createBuffer(bufferSize, &data.indices);

        bufferSize = sizeof(data.normals[0]) * data.normals.size();
        instance->normalBuffer = vulkanContext.bufferService.createBuffer(bufferSize, &data.normals);

        bufferSize = sizeof(data.uvs[0]) * data.uvs.size();
        instance->uvBuffer = vulkanContext.bufferService.createBuffer(bufferSize, &data.uvs);

        return instance;
    }
} // Metal
