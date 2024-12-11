#include "MeshService.h"

#include <glm/vec3.hpp>

#include "MeshData.h"
#include "MeshInstance.h"
#include "../../../context/VulkanContext.h"
#include "../../../context/repository/ResourceRepository.h"
#include "../../../context/runtime/BufferInstance.h"

namespace Metal {
    MeshInstance *MeshService::createMesh(const MeshData &data) const {
        auto *instance = new MeshInstance;
        registerResource(instance);
        instance->setVertexBuffer(vulkanContext.bufferService.createBuffer<glm::vec3>(data.vertices));
        instance->setIndexBuffer(vulkanContext.bufferService.createBuffer<uint32_t>(data.indices));
        instance->setNormalBuffer(vulkanContext.bufferService.createBuffer<glm::vec3>(data.normals));
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
