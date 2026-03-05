#include "MeshService.h"
#include "MeshInstance.h"
#include "MeshData.h"
#include "SceneData.h"
#include "EntityAssetData.h"

#include "../../context/vulkan/VulkanContext.h"
#include "../../util/FilesUtil.h"
#include "../../util/serialization-definitions.h"
#include <cereal/archives/binary.hpp>

#include <fstream>

#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/PrimitiveComponent.h"
#include "../../repository/world/components/TransformComponent.h"

namespace Metal {
    MeshInstance *MeshService::create(const std::string &id) {
        MeshData *data = stream(id);
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

    MeshData *MeshService::stream(const std::string &id) const {
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

    EntityID MeshService::createMeshEntity(const std::string &name, const std::string &meshId,
                                           const EntityAssetData *data) const {
        const auto id = CTX.worldRepository.createEntity();
        CTX.worldRepository.createComponent(id, ComponentTypes::ComponentType::PRIMITIVE);
        const auto entity = static_cast<entt::entity>(id);
        auto &mesh = CTX.worldRepository.registry.get<PrimitiveComponent>(entity);
        mesh.meshId = meshId;

        if (data != nullptr) {
            mesh.albedo = data->albedo;
            mesh.roughness = data->roughness;
            mesh.metallic = data->metallic;
            mesh.albedoColor = data->albedoColor;
            mesh.roughnessFactor = data->roughnessFactor;
            mesh.metallicFactor = data->metallicFactor;
            mesh.transmissionFactor = data->transmissionFactor;
            mesh.thicknessFactor = data->thicknessFactor;
            mesh.ior = data->ior;
            mesh.isEmissive = data->isEmissive;
        }

        MeshData *meshData = stream(meshId);
        if (meshData != nullptr) {
            auto &transform = CTX.worldRepository.registry.get<TransformComponent>(entity);
            transform.gizmoCenter = meshData->gizmoCenter;
            delete meshData;
        }

        CTX.worldRepository.getEntity(id)->name = name;
        return id;
    }

    void MeshService::createSceneEntities(const std::string &id) const {
        auto &repo = CTX.worldRepository;
        SceneData sceneData;
        auto pathToFile = CTX.getAssetDirectory() + FORMAT_FILE_SCENE(id);
        PARSE_TEMPLATE(sceneData, pathToFile)

        std::unordered_map<int, EntityID> entities;

        for (auto &entity: sceneData.entities) {
            if (!entity.meshId.empty()) {
                entities.insert({entity.id, createMeshEntity(entity.name, entity.meshId, &entity)});
            } else {
                const auto entityId = repo.createEntity();
                entities.insert({entity.id, entityId});
                repo.getEntity(entityId)->name = entity.name;
                repo.getEntity(entityId)->initialize(true);
            }
        }

        for (auto &entity: sceneData.entities) {
            if (entity.parentEntity < 0 || !entities.contains(entity.parentEntity)) {
                continue;
            }
            repo.linkEntities(entities.at(entity.parentEntity), entities.at(entity.id));
        }
    }

    void MeshService::disposeResource(MeshInstance *resource) {
        LOG_INFO("Disposing of mesh instance");
        CTX.rayTracingService.markDirty();
        CTX.bufferService.dispose(resource->indexBuffer->getId());
        CTX.bufferService.dispose(resource->dataBuffer->getId());
    }
} // Metal
