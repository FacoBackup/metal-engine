#ifndef CAPSULE_COLLIDER_COMPONENT_H
#define CAPSULE_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include "AbstractComponent.h"

namespace Metal {
    struct CapsuleColliderComponent final : AbstractComponent {
        float radius = 0.5f;
        float height = 1.0f;
        glm::vec3 offset{0.0f, 0.0f, 0.0f};

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}
#endif //CAPSULE_COLLIDER_COMPONENT_H
