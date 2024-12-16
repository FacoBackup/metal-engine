#include "MeshService.h"
#include "../runtime/MeshInstance.h"
#include "../runtime/MeshData.h"
#include "../VulkanContext.h"
#include "../../common/util/files/FilesUtil.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

#include "../ApplicationContext.h"
#include "../../engine/enum/LevelOfDetail.h"

namespace Metal {
    MeshInstance *MeshService::create(const std::string &id, const LevelOfDetail &levelOfDetail) {
        MeshData data;
        auto pathToFile = context.getAssetDirectory() + LevelOfDetail::GetFormattedName(id, levelOfDetail, EntryType::MESH);
        std::cout << id << " Loading " << pathToFile << std::endl;
        std::ifstream os(
            pathToFile,
            std::ios::binary);
        cereal::BinaryInputArchive archive(os);
        data.load(archive);

        auto *instance = new MeshInstance(id);
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
