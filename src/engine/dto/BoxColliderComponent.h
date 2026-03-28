#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include "AbstractComponent.h"
#include "../../common/Reflection.h"

namespace Metal {
    struct BoxColliderComponent final : AbstractComponent {
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
        glm::vec3 offset{0.0f, 0.0f, 0.0f};

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}
#endif //BOX_COLLIDER_COMPONENT_H
