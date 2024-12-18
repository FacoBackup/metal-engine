#include "WorldRepository.h"

#include "../../enum/ComponentType.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

namespace Metal {
    EntityID WorldRepository::createEntity(std::string name, const bool container) {
        lastId = entities.empty() ? ROOT_ID : lastId + 1;
        entities.insert({lastId, std::make_unique<Entity>(lastId, container)});
        auto *emplace = getEntity(lastId);
        emplace->isContainer = container;
        emplace->name = std::move(name);
        emplace->parent = lastId == ROOT_ID ? EMPTY_ENTITY : ROOT_ID;
        if (emplace->parent != EMPTY_ENTITY) {
            getEntity(emplace->parent)->children.push_back(emplace->getId());
        }
        return lastId;
    }

    void WorldRepository::linkEntities(const Entity *target, Entity *toLink) const {
        auto *parent = getEntity(toLink->parent);
        parent->children.erase(
            std::ranges::remove(parent->children, toLink->getId()).begin(),
            parent->children.end());

        const EntityID id = target != nullptr ? target->getId() : ROOT_ID;
        getEntity(id)->children.push_back(toLink->getId());
        toLink->parent = id;
    }

    Entity *WorldRepository::getEntity(const EntityID node) const {
        if (entities.contains(node)) {
            return entities.find(node)->second.get();
        }
        return nullptr;
    }

    WorldRepository::WorldRepository() {
        createEntity("Scene", true);
    }

    void WorldRepository::createComponent(const EntityID entity, ComponentTypes::ComponentType type)  {
        if (!entities.contains(entity)) {
            return;
        }
        switch (type) {
            case ComponentTypes::ComponentType::MESH: {
                auto *m = new MeshComponent(entity);
                getEntity(entity)->components.insert({
                    ComponentTypes::ComponentType::MESH, m
                });
                meshes[entity] = m;

                auto *t = new TransformComponent(entity);
                getEntity(entity)->components.insert({
                    ComponentTypes::ComponentType::TRANSFORM, t
                });
                transforms[entity] = t;
                break;
            }
            default:
                break;
        }
    }
} // Metal
