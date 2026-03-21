#ifndef SPHERE_COLLIDER_COMPONENT_H
#define SPHERE_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include "AbstractComponent.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct SphereColliderComponent final : AbstractComponent, ISerialize {
        float radius = 0.5f;
        glm::vec3 offset{0.0f, 0.0f, 0.0f};

        void registerFields() override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //SPHERE_COLLIDER_COMPONENT_H
