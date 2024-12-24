#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <unordered_map>
#include <vector>
#include <glm/ext/scalar_constants.hpp>

#include "../../enum/engine-definitions.h"
#include "../../service/camera/Camera.h"
#include "impl/Entity.h"
#include "../../enum/ComponentType.h"

namespace Metal {
    struct TransformComponent;
    struct MeshComponent;

    struct WorldRepository final {
        static constexpr EntityID ROOT_ID = 1;

        explicit WorldRepository();

        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        std::unordered_map<EntityID, std::unique_ptr<Entity> > entities{};
        std::unordered_map<EntityID, MeshComponent *> meshes{};
        std::unordered_map<EntityID, TransformComponent *> transforms{};
        std::unordered_map<EntityID, bool> culled{};
        std::unordered_map<EntityID, bool> hiddenEntities{};

        void createComponent(EntityID entity, ComponentTypes::ComponentType type);

        EntityID createEntity(std::string name = "New Entity", bool container = false);

        void linkEntities(const Entity *parentEntity, Entity *child) const;

        [[nodiscard]] Entity *getEntity(EntityID node) const;

    private:
        EntityID lastId = ROOT_ID;
    };
} // Metal

#endif //WORLDREPOSITORY_H
