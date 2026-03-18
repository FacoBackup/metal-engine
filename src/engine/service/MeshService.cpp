#include "MeshService.h"
#include "../resource/MeshInstance.h"
#include "../dto/MeshData.h"
#include "../../editor/dto/SceneData.h"

#include "../../core/VulkanContext.h"
#include "../../common/FilesUtil.h"
#include "../../common/LoggerUtil.h"
#include "../../core/DirectoryService.h"
#include <cereal/archives/binary.hpp>

#include <fstream>
#include <filesystem>

#include "../../ApplicationContext.h"
#include "BufferService.h"
#include "RayTracingService.h"
#include "../dto/PrimitiveComponent.h"
#include "../dto/TransformComponent.h"
#include "../repository/WorldRepository.h"

namespace Metal {

    void MeshService::onInitialize() {
        historyEventService->subscribe<PrimitiveComponent>([this](const HistoryEvent &event) {
            if (event.propertyPath.find("Mesh") != std::string::npos) {
                auto *primitive = dynamic_cast<PrimitiveComponent *>(event.instance);
                if (primitive && worldRepository->registry.all_of<TransformComponent>(primitive->getEntityId())) {
                    MeshData *data = loadMeshData(primitive->meshId);
                    if (data != nullptr) {
                        worldRepository->registry.get<TransformComponent>(primitive->getEntityId()).gizmoCenter = data->gizmoCenter;
                        delete data;
                    }
                }
            }
        }, "Mesh");
    }

    MeshInstance *MeshService::create(const std::string &id) {
        MeshData *data = loadMeshData(id);
        if (data == nullptr) {
            return nullptr;
        }
        auto *instance = createResourceInstance(id);

        instance->indexCount = data->indices.size();
        instance->vertexCount = data->data.size();

        instance->dataBuffer = bufferService->createBuffer(
            id + "_data",
            sizeof(VertexData) * data->data.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            data->data.data(),
            true);

        instance->indexBuffer = bufferService->createBuffer(
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
        auto pathToFile = directoryService->getRootDirectory() + "/assets/" + FORMAT_FILE_MESH(id);
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
        bufferService->dispose(resource->indexBuffer->getId());
        bufferService->dispose(resource->dataBuffer->getId());
    }
} // Metal
