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
#include "../../enum/LevelOfDetail.h"
#include "../../repository/world/components/MeshComponent.h"
#include "../../repository/world/components/TransformComponent.h"

namespace Metal {
    MeshInstance *MeshService::create(const std::string &id, const LevelOfDetail &levelOfDetail) {
        MeshData *data = stream(id, levelOfDetail);
        if (data == nullptr) {
            return nullptr;
        }
        auto *instance = new MeshInstance(id + levelOfDetail.suffix);
        registerResource(instance);

        instance->indexCount = data->indices.size();
        instance->vertexCount = data->data.size();

        instance->dataBuffer = CTX.bufferService.createBuffer(
            sizeof(VertexData) * data->data.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
            data->data.data(),
            true);

        instance->indexBuffer = CTX.bufferService.createBuffer(
            sizeof(unsigned int) * data->indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
            data->indices.data(),
            true);

        delete data;

        return instance;
    }

    MeshData *MeshService::stream(const std::string &id, const LevelOfDetail &levelOfDetail) const {
        auto pathToFile = CTX.getAssetDirectory() + FORMAT_FILE_MESH(id, levelOfDetail);
        if (std::filesystem::exists(pathToFile)) {
            auto *data = new MeshData;
            std::ifstream input(pathToFile, std::ios::binary);
            cereal::BinaryInputArchive archive(input);
            archive(*data);
            return data;
        }
        return nullptr;
    }

    EntityID MeshService::createMeshEntity(const std::string &name, const std::string &meshId, const std::string &materialId) const {
        const auto id = CTX.worldRepository.createEntity();
        CTX.worldRepository.createComponent(id, ComponentTypes::ComponentType::MESH);
        const auto entity = static_cast<entt::entity>(id);
        auto &mesh = CTX.worldRepository.registry.get<MeshComponent>(entity);
        mesh.meshId = meshId;
        mesh.materialId = materialId;

        MeshData *data = stream(meshId, LevelOfDetail::LOD_0);
        if (data != nullptr) {
            auto &transform = CTX.worldRepository.registry.get<TransformComponent>(entity);
            transform.gizmoCenter = data->gizmoCenter;
            delete data;
        }

        CTX.worldRepository.getEntity(id)->name = name;
        return id;
    }

    void MeshService::createSceneEntities(const std::string &id) const {
        auto &repo = CTX.worldRepository;
        SceneData data;
        auto pathToFile = CTX.getAssetDirectory() + FORMAT_FILE_SCENE(id);
        PARSE_TEMPLATE(data, pathToFile)

        std::unordered_map<int, EntityID> entities;

        for (auto &entity: data.entities) {
            if (!entity.meshId.empty()) {
                entities.insert({entity.id, createMeshEntity(entity.name, entity.meshId, entity.materialId)});
            } else {
                const auto entityId = repo.createEntity();
                entities.insert({entity.id, entityId});
                repo.getEntity(entityId)->name = entity.name;
                repo.getEntity(entityId)->initialize(true);
            }
        }

        for (auto &entity: data.entities) {
            if (entity.parentEntity < 0 || !entities.contains(entity.parentEntity)) {
                continue;
            }
            repo.linkEntities(entities.at(entity.parentEntity), entities.at(entity.id));
        }
    }
} // Metal
