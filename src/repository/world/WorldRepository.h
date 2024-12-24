#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <unordered_map>
#include <vector>
#include <glm/ext/scalar_constants.hpp>

#include "../../enum/engine-definitions.h"
#include "../../service/camera/Camera.h"
#include "impl/Entity.h"
#include "../../enum/ComponentType.h"
#include "../../util/serialization-definitions.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

namespace Metal {
    struct WorldRepository final {
        static constexpr EntityID ROOT_ID = 1;

        explicit WorldRepository();
        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        std::unordered_map<EntityID, Entity> entities{};
        std::unordered_map<EntityID, MeshComponent> meshes;
        std::unordered_map<EntityID, TransformComponent> transforms;
        std::unordered_map<EntityID, bool> culled{};
        std::unordered_map<EntityID, bool> hiddenEntities{};

        SAVE_TEMPLATE(lastId, culled, hiddenEntities, meshes, transforms, camera, entities)

        void createComponent(EntityID entity, ComponentTypes::ComponentType type);

        EntityID createEntity(std::string name = "New Entity", bool container = false);

        void linkEntities(const Entity *parentEntity, Entity *child);

        [[nodiscard]] Entity *getEntity(EntityID node);

        Inspectable *getComponent(ComponentTypes::ComponentType comp, EntityID entity);

    private:
        EntityID lastId = ROOT_ID;
    };
} // Metal

#endif //WORLDREPOSITORY_H
