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

namespace Metal {
    MeshInstance *MeshService::create(const std::string &id, const LevelOfDetail &levelOfDetail) {
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_MESH(id, levelOfDetail);
        MeshData data;
        PARSE_TEMPLATE(data.load, pathToFile)

        auto *instance = new MeshInstance(id);
        registerResource(instance);

        instance->indexCount = data.indices.size();

        instance->dataBuffer = context.bufferService.createBuffer(
            sizeof(VertexData) * data.data.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            data.data.data());

        instance->indexBuffer = context.bufferService.createBuffer(
            sizeof(uint32_t) * data.indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            data.indices.data());

        return instance;
    }

    EntityID MeshService::loadMesh(const std::string &name, const std::string &meshId) const {
        const auto id = context.worldRepository.createEntity();
        context.worldRepository.createComponent(id, ComponentTypes::ComponentType::MESH);
        context.worldRepository.meshes[id]->meshId = meshId;
        context.worldRepository.getEntity(id)->name = name;
        return id;
    }

    void MeshService::loadScene(const std::string &id) const {
        auto &repo = context.worldRepository;
        SceneData data;
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_SCENE(id);
        std::ifstream os(pathToFile, std::ios::binary);
        cereal::BinaryInputArchive archive(os);
        data.load(archive);

        std::unordered_map<int, EntityID> entities;

        for (auto &entity: data.entities) {
            if (!entity.meshId.empty()) {
                entities.insert({entity.id, loadMesh(entity.name, entity.meshId)});
            } else {
                const auto entityId = repo.createEntity();
                entities.insert({entity.id, entityId});
                repo.getEntity(entityId)->name = entity.name;
                repo.getEntity(entityId)->isContainer = true;
            }
        }

        for (auto &entity: data.entities) {
            if (entity.parentEntity < 0) {
                continue;
            }
            repo.linkEntities(repo.getEntity(entities.at(entity.parentEntity)), repo.getEntity(entities.at(entity.id)));
        }
    }
} // Metal
