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
#include "impl/MetadataComponent.h"
#include "../../enum/ComponentType.h"
#include "components/SphereLightComponent.h"
#include "components/PlaneLightComponent.h"
#include "components/TransformComponent.h"
#include "components/PrimitiveComponent.h"
#include "components/VolumeComponent.h"
#include "components/AtmosphereComponent.h"

namespace Metal {
    class Inspectable;

    struct WorldRepository final : AbstractRuntimeComponent, Serializable {
        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        entt::registry registry{};
        std::unordered_map<entt::entity, bool> culled{};
        std::unordered_map<entt::entity, bool> hiddenEntities{};

        void createComponent(entt::entity entity, ComponentType type);

        entt::entity createEntity();

        [[nodiscard]] MetadataComponent *getEntity(entt::entity node);

        void deleteEntities(const std::vector<entt::entity> &entities);

        void changeVisibility(entt::entity entity, bool isVisible);

        void loadScene(const std::string &sceneId);

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;

    };
} // Metal

#endif //WORLDREPOSITORY_H
