#include "MeshService.h"
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
    EntityID MeshService::createMeshEntity(const std::string &name, const std::string &meshId) const {
        const auto id = context.worldRepository.createEntity();
        context.worldRepository.createComponent(id, ComponentTypes::ComponentType::MESH);
        context.worldRepository.meshes[id].meshId = meshId;
        context.worldRepository.getEntity(id)->name = name;
        context.engineContext.dispatchBVHBuild();
        return id;
    }

    MeshData *MeshService::stream(const std::string &id, const LevelOfDetail &levelOfDetail) const {
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_MESH(id, levelOfDetail);
        if (std::filesystem::exists(pathToFile)) {
            MeshData *data = new MeshData;
            PARSE_TEMPLATE(data->load, pathToFile)
            return data;
        }
        return nullptr;
    }
    void MeshService::createSceneEntities(const std::string &id) const {
        auto &repo = context.worldRepository;
        SceneData data;
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_SCENE(id);
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

        context.engineContext.dispatchBVHBuild();
    }
} // Metal
