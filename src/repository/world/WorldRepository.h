#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <memory>
#include <vector>
#include <glm/ext/scalar_constants.hpp>
#include <entt/entt.hpp>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../util/Serializable.h"
#include "../../enum/engine-definitions.h"
#include "../../service/camera/Camera.h"
#include "impl/EntityComponent.h"
#include "../../enum/ComponentType.h"
#include "components/LightComponent.h"
#include "components/PrimitiveComponent.h"
#include "components/SphereLightComponent.h"
#include "components/PlaneLightComponent.h"
#include "components/TransformComponent.h"
#include "components/VolumeComponent.h"
#include "components/AtmosphereComponent.h"

namespace Metal {
    class Inspectable;

    struct WorldRepository final : AbstractRuntimeComponent, Serializable {
        static constexpr EntityID ROOT_ID = 1;

        explicit WorldRepository();

        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        entt::registry registry{};
        std::unordered_map<EntityID, bool> culled{};
        std::unordered_map<EntityID, bool> hiddenEntities{};

        void createComponent(EntityID entity, ComponentTypes::ComponentType type);

        EntityID createEntity(std::string name = "New Entity", bool container = false);

        void linkEntities(EntityID parentId, EntityID childId);

        [[nodiscard]] EntityComponent *getEntity(EntityID node);

        void deleteEntities(const std::vector<EntityID> &entities);

        void changeVisibility(EntityID entity, bool isVisible);

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;

    private:
        void deleteRecursively(const std::vector<EntityID> &entities);

        void changeVisibilityRecursively(EntityID entity, bool isVisible);
    };
} // Metal

#endif //WORLDREPOSITORY_H
