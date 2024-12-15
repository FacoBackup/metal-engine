#include "WorldRepository.h"

#include "../../enum/ComponentType.h"
#include "../../../common/util/Util.h"
#include "impl/MeshComponent.h"
#include "../camera/Camera.h"
#include "impl/TransformComponent.h"

namespace Metal {
    EntityID WorldRepository::createEntity(std::string name, const bool container) {
        auto id = entities.empty() ? ROOT_ID.c_str() : Util::uuidV4().c_str();
        entities.insert({id, std::make_unique<Entity>(id)});
        auto *emplace = getEntity(id);
        emplace->isContainer = container;
        emplace->name = std::move(name);
        return id;
    }

    void WorldRepository::linkEntities(const Entity *target, Entity *toLink) const {
        auto *parent = getEntity(toLink->parent);
        parent->children.erase(
            std::ranges::remove(parent->children, toLink->getId()).begin(),
            parent->children.end());

        const EntityID id = target != nullptr ? target->getId() : ROOT_ID.c_str();
        getEntity(id)->children.push_back(toLink->getId());
        toLink->parent = id;
    }

    Entity *WorldRepository::getEntity(const EntityID node) const {
        if (entities.contains(node)) {
            return entities.find(node)->second.get();
        }
        return nullptr;
    }

    WorldRepository::WorldRepository(): camera(new Camera()) {
    }

    void WorldRepository::onInitialize() {
        createEntity("Scene", true);
    }

    void WorldRepository::createComponent(const EntityID entity, ComponentTypes::ComponentType type) const {
        if (!entities.contains(entity)) {
            return;
        }
        switch (type) {
            case ComponentTypes::ComponentType::MESH:
                getEntity(entity)->components.insert({type, std::make_unique<MeshComponent>()});
                getEntity(entity)->components.insert({type, std::make_unique<TransformComponent>()});
                break;
            default:
                break;
        }
    }
} // Metal
