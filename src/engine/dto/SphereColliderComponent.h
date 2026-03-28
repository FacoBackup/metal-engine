#ifndef SPHERE_COLLIDER_COMPONENT_H
#define SPHERE_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include "AbstractComponent.h"


namespace Metal {
    struct SphereColliderComponent final : AbstractComponent {
        float radius = 0.5f;
        glm::vec3 offset{0.0f, 0.0f, 0.0f};

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}
#endif //SPHERE_COLLIDER_COMPONENT_H
