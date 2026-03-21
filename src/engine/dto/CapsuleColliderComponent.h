#ifndef CAPSULE_COLLIDER_COMPONENT_H
#define CAPSULE_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include "AbstractComponent.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct CapsuleColliderComponent final : AbstractComponent, ISerialize {
        float radius = 0.5f;
        float height = 1.0f;
        glm::vec3 offset{0.0f, 0.0f, 0.0f};

        void registerFields() override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //CAPSULE_COLLIDER_COMPONENT_H
