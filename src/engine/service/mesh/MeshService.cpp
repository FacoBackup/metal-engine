#include "MeshService.h"

#include "MeshData.h"
#include "MeshInstance.h"
#include "../../../context/VulkanContext.h"
#include "../../../context/repository/ResourceRepository.h"
#include "../../../context/runtime/BufferInstance.h"

namespace Metal {
    MeshInstance *MeshService::createMesh(const MeshData &data) const {
        auto *instance = new MeshInstance;
        registerResource(instance);

        VkDeviceSize bufferSize = sizeof(data.vertices[0]) * data.vertices.size();
        instance->setVertexBuffer(vulkanContext.bufferService.createBuffer(bufferSize, &data.vertices));

        bufferSize = sizeof(data.indices[0]) * data.indices.size();
        instance->setIndexBuffer(vulkanContext.bufferService.createBuffer(bufferSize, &data.indices));

        bufferSize = sizeof(data.normals[0]) * data.normals.size();
        instance->setNormalBuffer(vulkanContext.bufferService.createBuffer(bufferSize, &data.normals));

        bufferSize = sizeof(data.uvs[0]) * data.uvs.size();
        instance->setNormalBuffer(vulkanContext.bufferService.createBuffer(bufferSize, &data.uvs));
        return instance;
    }

    void MeshService::dispose(MeshInstance *instance) const {
        repository.resources.erase(instance->getNormalBuffer()->id);
        repository.resources.erase(instance->getVertexBuffer()->id);
        repository.resources.erase(instance->getIndexBuffer()->id);
        repository.resources.erase(instance->id);

        instance->dispose(vulkanContext);
        delete instance->getNormalBuffer();
        delete instance->getVertexBuffer();
        delete instance->getIndexBuffer();
        delete instance;
    }
} // Metal
