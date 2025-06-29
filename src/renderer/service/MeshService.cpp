#include "MeshService.h"
#include "../data/MeshData.h"
#include "../data/SceneData.h"
#include "../data/EntityAssetData.h"

#include "../VulkanContext.h"
#include "../../common/FilesUtil.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

#include "../../MetalContextProvider.h"
#include "../../common/enum/LevelOfDetail.h"
#include "../../runtime/data/components/MeshComponent.h"

namespace Metal {
    EntityID MeshService::createMeshEntity(const std::string &name, const std::string &meshId) const {
        const auto id = SINGLETONS.worldRepository.createEntity();
        SINGLETONS.worldRepository.createComponent(id, ComponentTypes::ComponentType::MESH);
        SINGLETONS.worldRepository.meshes[id].meshId = meshId;
        SINGLETONS.worldRepository.getEntity(id)->name = name;
        SINGLETONS.engineContext.dispatchBVHBuild();
        return id;
    }

    MeshData *MeshService::stream(const std::string &id, const LevelOfDetail &levelOfDetail) const {
        auto pathToFile = ContextProvider::Get().getAssetDirectory() + FORMAT_FILE_MESH(id, levelOfDetail);
        if (std::filesystem::exists(pathToFile)) {
            MeshData *data = new MeshData;
            PARSE_TEMPLATE(data->load, pathToFile)
            return data;
        }
        return nullptr;
    }
    void MeshService::createSceneEntities(const std::string &id) const {
        auto &repo = SINGLETONS.worldRepository;
        SceneData data;
        auto pathToFile = ContextProvider::Get().getAssetDirectory() + FORMAT_FILE_SCENE(id);
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

        SINGLETONS.engineContext.dispatchBVHBuild();
    }
} // Metal
