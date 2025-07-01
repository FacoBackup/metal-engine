#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <unordered_map>
#include <vector>
#include <glm/ext/scalar_constants.hpp>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../common/engine-definitions.h"
#include "../data/Camera.h"
#include "../data/Entity.h"
#include "../../common/enum/ComponentType.h"
#include "../../common/serialization-definitions.h"
#include "../data/components/LightComponent.h"
#include "../data/components/MeshComponent.h"
#include "../data/components/TransformComponent.h"
#include "../data/components/VolumeComponent.h"
#include "../data/components/CameraComponent.h"

namespace Metal {
    struct WorldRepository final : AbstractRuntimeComponent{
        static constexpr EntityID ROOT_ID = 1;

        explicit WorldRepository();

        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        std::unordered_map<EntityID, Entity> entities{};
        std::unordered_map<EntityID, MeshComponent> meshes{};
        std::unordered_map<EntityID, TransformComponent> transforms{};
        std::unordered_map<EntityID, LightComponent> lights{};
        std::unordered_map<EntityID, VolumeComponent> volumes{};
        std::unordered_map<EntityID, CameraComponent> cameras{};
        std::unordered_map<EntityID, bool> culled{};
        std::unordered_map<EntityID, bool> hiddenEntities{};

        SAVE_TEMPLATE(lastId, culled, hiddenEntities, meshes, transforms, camera, lights, volumes, entities, cameras)

        void createComponent(EntityID entity, ComponentTypes::ComponentType type);

        EntityID createEntity(std::string name = "New Entity", bool container = false);

        void linkEntities(const Entity *parentEntity, Entity *child);

        [[nodiscard]] Entity *getEntity(EntityID node);

        Inspectable *getComponent(ComponentTypes::ComponentType comp, EntityID entity);

        void deleteRecursively(const std::vector<EntityID> &entities);

        void deleteEntities(const std::vector<EntityID> & entities);

        void changeVisibility(EntityID entity, bool isHidden);

    private:
        void changeVisibilityRecursively(EntityID entity, bool isHidden);

        EntityID lastId = ROOT_ID;
    };
} // Metal

#endif //WORLDREPOSITORY_H
