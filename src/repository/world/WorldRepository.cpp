#include "WorldRepository.h"

#include "../../context/ApplicationContext.h"
#include "../../enum/ComponentType.h"

namespace Metal {
    WorldRepository::WorldRepository(ApplicationContext &context): AbstractRuntimeComponent(context) {
        entities.emplace(ROOT_ID, Entity{});
        entities.at(ROOT_ID).initialize(lastId, true);

        auto *emplace = getEntity(lastId);
        emplace->name = "Scene";
    }

    EntityID WorldRepository::createEntity(std::string name, const bool container) {
        lastId++;
        entities.emplace(lastId, Entity{});
        auto *emplace = getEntity(lastId);
        emplace->initialize(lastId, container);
        emplace->name = std::move(name);
        emplace->parent = ROOT_ID;
        getEntity(emplace->parent)->children.push_back(emplace->getId());
        return lastId;
    }

    void WorldRepository::linkEntities(const Entity *parentEntity, Entity *child) {
        auto *parent = getEntity(child->parent);
        parent->children.erase(
            std::ranges::remove(parent->children, child->getId()).begin(),
            parent->children.end());

        const EntityID id = parentEntity != nullptr ? parentEntity->getId() : ROOT_ID;
        getEntity(id)->children.push_back(child->getId());
        child->parent = id;
    }

    Entity *WorldRepository::getEntity(const EntityID node) {
        if (entities.contains(node)) {
            return &entities.find(node)->second;
        }
        return nullptr;
    }

    Inspectable *WorldRepository::getComponent(ComponentTypes::ComponentType comp, EntityID entity) {
        switch (comp) {
            case ComponentTypes::MESH:
                return meshes.contains(entity) ? &meshes.find(entity)->second : nullptr;
            case ComponentTypes::TRANSFORM:
                return transforms.contains(entity) ? &transforms.find(entity)->second : nullptr;
            case ComponentTypes::LIGHT:
                return lights.contains(entity) ? &lights.find(entity)->second : nullptr;
            default:
                return nullptr;
        }
    }

    void WorldRepository::createComponent(const EntityID entity, ComponentTypes::ComponentType type) {
        if (!entities.contains(entity)) {
            return;
        }
        switch (type) {
            case ComponentTypes::MESH: {
                meshes.emplace(entity, MeshComponent{});
                meshes.at(entity).setEntityId(entity);
                getEntity(entity)->components.push_back(ComponentTypes::MESH);
                createComponent(entity, ComponentTypes::TRANSFORM);
                break;
            }
            case ComponentTypes::LIGHT: {
                lights.emplace(entity, LightComponent{});
                lights.at(entity).setEntityId(entity);
                getEntity(entity)->components.push_back(ComponentTypes::LIGHT);
                createComponent(entity, ComponentTypes::TRANSFORM);
                context.engineContext.setUpdateLights();
                break;
            }
            case ComponentTypes::TRANSFORM: {
                transforms.emplace(entity, TransformComponent{});
                transforms.at(entity).setEntityId(entity);
                getEntity(entity)->components.push_back(ComponentTypes::TRANSFORM);
                context.worldGridRepository.getCurrentTile()->entities.push_back(entity);
                entities.at(entity).onTile = context.worldGridRepository.getCurrentTile()->id;
                break;
            }
            default:
                break;
        }
    }
} // Metal
