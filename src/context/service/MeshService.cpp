#include "MeshService.h"
#include "../runtime/MeshInstance.h"
#include "../runtime/MeshData.h"
#include "../VulkanContext.h"
#include "../../common/util/files/FilesUtil.h"

#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

namespace Metal {
    MeshInstance *MeshService::createMesh(const std::string &path) {
        MeshData data;

        std::ifstream os(path, std::ios::binary);
        cereal::BinaryInputArchive archive(os);
        data.load(archive);

        auto *instance = new MeshInstance;
        registerResource(instance);

        instance->indexCount = data.indices.size();
        instance->dataBuffer = vulkanContext.bufferService.createBuffer(
            sizeof(VertexData) * data.data.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            data.data.data());

        instance->indexBuffer = vulkanContext.bufferService.createBuffer(
            sizeof(uint32_t) * data.indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            data.indices.data());

        return instance;
    }
} // Metal
