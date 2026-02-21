#include "MeshService.h"
#include "MeshInstance.h"
#include "MeshData.h"
#include "SceneData.h"
#include "EntityAssetData.h"

#include "../../context/vulkan/VulkanContext.h"
#include "../../util/FilesUtil.h"

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

        const VkBufferUsageFlags rtVertexFlags = (VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
        const VkBufferUsageFlags rtIndexFlags = rtVertexFlags;

        instance->dataBuffer = CTX.bufferService.createBuffer(
            sizeof(VertexData) * data->data.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | rtVertexFlags,
            data->data.data());

        instance->indexBuffer = CTX.bufferService.createBuffer(
            sizeof(unsigned int) * data->indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | rtIndexFlags,
            data->indices.data());

        delete data;

        return instance;
    }

    MeshData *MeshService::stream(const std::string &id, const LevelOfDetail &levelOfDetail) const {
        auto pathToFile = CTX.getAssetDirectory() + FORMAT_FILE_MESH(id, levelOfDetail);
        if (std::filesystem::exists(pathToFile)) {
            MeshData *data = new MeshData;
            PARSE_TEMPLATE(data->load, pathToFile)
            return data;
        }
        return nullptr;
    }

    EntityID MeshService::createMeshEntity(const std::string &name, const std::string &meshId) const {
        const auto id = CTX.worldRepository.createEntity();
        CTX.worldRepository.createComponent(id, ComponentTypes::ComponentType::MESH);
        auto &mesh = CTX.worldRepository.meshes[id];
        mesh.meshId = meshId;

        MeshData *data = stream(meshId, LevelOfDetail::LOD_0);
        if (data != nullptr) {
            auto &transform = CTX.worldRepository.transforms[id];
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
        std::ifstream os(pathToFile, std::ios::binary);
        cereal::BinaryInputArchive archive(os);
        data.load(archive);

        std::unordered_map<int, EntityID> entities;

        for (auto &entity: data.entities) {
            if (!entity.meshId.empty()) {
                entities.insert({entity.id, createMeshEntity(entity.name, entity.meshId)});
            } else {
                const auto entityId = repo.createEntity();
                entities.insert({entity.id, entityId});
                repo.getEntity(entityId)->name = entity.name;
                repo.getEntity(entityId)->isContainer = true;
            }
        }

        for (auto &entity: data.entities) {
            if (entity.parentEntity < 0 || !entities.contains(entity.parentEntity)) {
                continue;
            }
            repo.linkEntities(repo.getEntity(entities.at(entity.parentEntity)), repo.getEntity(entities.at(entity.id)));
        }
    }
} // Metal
