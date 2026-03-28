#include <engine/service/MeshService.h>
#include <engine/resource/MeshInstance.h>
#include <engine/dto/MeshData.h>
#include <editor/dto/SceneData.h>
#include <core/VulkanContext.h>
#include <common/FilesUtil.h>
#include <common/LoggerUtil.h>
#include <core/DirectoryService.h>
#include <ApplicationContext.h>
#include <engine/service/BufferService.h>
#include <editor/service/HistoryService.h>
#include <engine/dto/PrimitiveComponent.h>
#include <engine/dto/TransformComponent.h>
#include <engine/repository/WorldRepository.h>
#include <editor/dto/FieldModificationEvent.h>
#include <engine/dto/ResourceDisposalPayload.h>
#include <ApplicationEventContext.h>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <filesystem>

namespace Metal {
    void MeshService::onInitialize() {
        eventListener([this](const Event &event) {
            auto payload = std::dynamic_pointer_cast<FieldModificationPayload>(event.payload);
            if (payload && payload->member.path.find("Mesh") != std::string::npos) {
                auto *primitive = dynamic_cast<PrimitiveComponent *>(payload->member.instance);
                if (primitive && worldRepository->registry.all_of<TransformComponent>(primitive->getEntityId())) {
                    MeshData *data = loadMeshData(primitive->meshId);
                    if (data != nullptr) {
                        worldRepository->registry.get<TransformComponent>(primitive->getEntityId()).gizmoCenter = data->
                                gizmoCenter;
                        delete data;
                    }
                }
            }
        }, "PrimitiveComponent");

        eventListener([this](const Event &event) {
            if (const auto payload = std::dynamic_pointer_cast<ResourceDisposalPayload>(event.payload)) {
                if (const MeshInstance *resource = getResource(payload->resourceId); resource != nullptr) {
                    LOG_INFO("Disposing of mesh instance");
                    bufferService->dispose(resource->indexBuffer->getId());
                    bufferService->dispose(resource->dataBuffer->getId());
                }
            }
        }, "RESOURCE_DISPOSAL");
    }

    MeshInstance *MeshService::create(const std::string &meshPath) {
        MeshData *data = loadMeshData(meshPath);
        if (data == nullptr) {
            return nullptr;
        }
        auto *instance = createResourceInstance(meshPath);

        instance->indexCount = data->indices.size();
        instance->vertexCount = data->data.size();

        instance->dataBuffer = bufferService->createBuffer(
            meshPath + "_data",
            sizeof(VertexData) * data->data.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            data->data.data(),
            true);

        instance->indexBuffer = bufferService->createBuffer(
            meshPath + "_indices",
            sizeof(unsigned int) * data->indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            data->indices.data(),
            true);

        delete data;

        ApplicationEventContext::dispatch("MeshLoaded");

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

    MeshData *MeshService::loadMeshData(const std::string &meshPath) const {
        if (std::filesystem::exists(meshPath)) {
            auto *data = new MeshData;
            std::ifstream input(meshPath, std::ios::binary);
            if (!input.is_open()) return nullptr;
            cereal::BinaryInputArchive archive(input);
            archive(*data);
            return data;
        }
        return nullptr;
    }

    void MeshService::disposeResource(MeshInstance *resource) {
    }
} // Metal
