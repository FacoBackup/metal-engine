#include "MeshService.h"
#include "../resource/MeshInstance.h"
#include "../dto/MeshData.h"
#include "../../editor/dto/SceneData.h"

#include "../../core/vulkan/VulkanContext.h"
#include "../../editor/util/FilesUtil.h"
#include <cereal/archives/binary.hpp>

#include <fstream>

#include "../../ApplicationContext.h"

namespace Metal {
    MeshInstance *MeshService::create(const std::string &id) {
        MeshData *data = loadMeshData(id);
        if (data == nullptr) {
            return nullptr;
        }
        auto *instance = createResourceInstance(id);

        instance->indexCount = data->indices.size();
        instance->vertexCount = data->data.size();

        instance->dataBuffer = CTX.bufferService.createBuffer(
            id + "_data",
            sizeof(VertexData) * data->data.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            data->data.data(),
            true);

        instance->indexBuffer = CTX.bufferService.createBuffer(
            id + "_indices",
            sizeof(unsigned int) * data->indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            data->indices.data(),
            true);

        delete data;

        return instance;
    }

    MeshInstance *MeshService::stream(const std::string &id) {
        if (id.empty()) {
            return nullptr;
        }
        auto *resource = getResource(id);
        if (resource != nullptr) {
            return resource;
        }
        return create(id);
    }

    MeshData *MeshService::loadMeshData(const std::string &id) const {
        auto pathToFile = CTX.getAssetDirectory() + FORMAT_FILE_MESH(id);
        if (std::filesystem::exists(pathToFile)) {
            auto *data = new MeshData;
            std::ifstream input(pathToFile, std::ios::binary);
            cereal::BinaryInputArchive archive(input);
            archive(*data);
            return data;
        }
        return nullptr;
    }

    void MeshService::disposeResource(MeshInstance *resource) {
        LOG_INFO("Disposing of mesh instance");
        CTX.rayTracingService.markDirty();
        CTX.bufferService.dispose(resource->indexBuffer->getId());
        CTX.bufferService.dispose(resource->dataBuffer->getId());
    }
} // Metal
