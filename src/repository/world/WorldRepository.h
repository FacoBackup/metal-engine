#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <unordered_map>
#include <vector>
#include <glm/ext/scalar_constants.hpp>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../enum/engine-definitions.h"
#include "../../service/camera/Camera.h"
#include "impl/Entity.h"
#include "../../enum/ComponentType.h"
#include "../../util/serialization-definitions.h"
#include "components/LightComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

namespace Metal {
    struct WorldRepository final : AbstractRuntimeComponent{
        static constexpr EntityID ROOT_ID = 1;

        explicit WorldRepository(ApplicationContext &context);

        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        std::unordered_map<EntityID, Entity> entities{};
        std::unordered_map<EntityID, MeshComponent> meshes{};
        std::unordered_map<EntityID, TransformComponent> transforms{};
        std::unordered_map<EntityID, LightComponent> lights{};
        std::unordered_map<EntityID, bool> culled{};
        std::unordered_map<EntityID, bool> hiddenEntities{};

        SAVE_TEMPLATE(lastId, culled, hiddenEntities, meshes, transforms, camera, lights, entities)

        void createComponent(EntityID entity, ComponentTypes::ComponentType type);

        EntityID createEntity(std::string name = "New Entity", bool container = false);

        void linkEntities(const Entity *parentEntity, Entity *child);

        [[nodiscard]] Entity *getEntity(EntityID node);

        Inspectable *getComponent(ComponentTypes::ComponentType comp, EntityID entity);

        void deleteEntities(const std::vector<EntityID> & entities);

        void changeVisibility(EntityID entity, bool isVisible);

    private:
        void changeVisibilityRecursively(EntityID entity, bool isVisible);

        EntityID lastId = ROOT_ID;
    };
} // Metal

#endif //WORLDREPOSITORY_H
