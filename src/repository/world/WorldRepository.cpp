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
            case ComponentTypes::VOLUME:
                return volumes.contains(entity) ? &volumes.find(entity)->second : nullptr;
            default:
                return nullptr;
        }
    }

    void WorldRepository::deleteRecursively(const std::vector<EntityID> &entities) {
        for (EntityID entity: entities) {
            if (lights.contains(entity)) {
                lights.erase(entity);
            }
            if (volumes.contains(entity)) {
                volumes.erase(entity);
            }
            if (transforms.contains(entity)) {
                transforms.erase(entity);
            }
            if (meshes.contains(entity)) {
                meshes.erase(entity);
            }
            if (hiddenEntities.contains(entity)) {
                hiddenEntities.erase(entity);
            }
            if (culled.contains(entity)) {
                culled.erase(entity);
            }
            if (this->entities.contains(entity)) {
                auto &currentEntity = this->entities.at(entity);
                if (currentEntity.children.size() > 0) {
                    std::vector<EntityID> newEntities = currentEntity.children;
                    deleteRecursively(newEntities);
                }

                auto parentId = currentEntity.parent;
                if (this->entities.contains(parentId)) {
                    auto &parent = this->entities.at(parentId);
                    parent.children.erase(
                        std::ranges::remove(parent.children, entity).begin(),
                        parent.children.end());
                }
                this->entities.erase(entity);
            }
        }
    }

    void WorldRepository::deleteEntities(const std::vector<EntityID> &entities) {
        deleteRecursively(entities);
        context.engineContext.setLightVolumeDataNeedsUpdate(true);
    }

    void WorldRepository::changeVisibility(EntityID entity, bool isHidden) {
        changeVisibilityRecursively(entity, isHidden);
        context.engineContext.setLightVolumeDataNeedsUpdate(true);
        context.engineContext.dispatchBVHBuild();
    }

    void WorldRepository::changeVisibilityRecursively(EntityID entity, const bool isHidden) {
        if (!isHidden) {
            hiddenEntities.erase(entity); // Remove from map to show it
        } else {
            hiddenEntities[entity] = true; // Ensure it's explicitly marked as hidden
        }

        for (const auto child: getEntity(entity)->children) {
            changeVisibilityRecursively(child, isHidden);
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
                context.engineContext.setLightVolumeDataNeedsUpdate(true);
                break;
            }
            case ComponentTypes::VOLUME: {
                volumes.emplace(entity, VolumeComponent{});
                volumes.at(entity).setEntityId(entity);
                getEntity(entity)->components.push_back(ComponentTypes::VOLUME);
                createComponent(entity, ComponentTypes::TRANSFORM);
                context.engineContext.setLightVolumeDataNeedsUpdate(true);
                break;
            }
            case ComponentTypes::TRANSFORM: {
                transforms.emplace(entity, TransformComponent{});
                transforms.at(entity).setEntityId(entity);
                getEntity(entity)->components.push_back(ComponentTypes::TRANSFORM);
                break;
            }
            default:
                break;
        }
    }
} // Metal
