#include "MeshService.h"
#include "../runtime/MeshInstance.h"
#include "../runtime/assets/MeshData.h"
#include "../runtime/assets/SceneData.h"
#include "../VulkanContext.h"
#include "../../common/util/files/FilesUtil.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

#include "../ApplicationContext.h"
#include "../../engine/enum/LevelOfDetail.h"
#include "../../engine/service/world/components/MeshComponent.h"
#include "../runtime/assets/EntityAssetData.h"

namespace Metal {
    MeshInstance *MeshService::create(const std::string &id, const LevelOfDetail &levelOfDetail) {
        MeshData data;
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_MESH(id, levelOfDetail);
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

    EntityID MeshService::loadMesh(const std::string &name, const std::string &meshId) const {
        const auto id = context.getEngineContext().worldRepository.createEntity();
        context.getEngineContext().worldRepository.createComponent(id, ComponentTypes::ComponentType::MESH);
        context.getEngineContext().worldRepository.meshes[id]->meshId = meshId;
        context.getEngineContext().worldRepository.getEntity(id)->name = name;
        return id;
    }

    void MeshService::loadScene(const std::string &id) const {
        auto &repo = context.getEngineContext().worldRepository;
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
