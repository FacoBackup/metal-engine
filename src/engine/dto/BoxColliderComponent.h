#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include "AbstractComponent.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct BoxColliderComponent final : AbstractComponent, ISerialize {
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
        glm::vec3 offset{0.0f, 0.0f, 0.0f};

        void registerFields() override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //BOX_COLLIDER_COMPONENT_H
