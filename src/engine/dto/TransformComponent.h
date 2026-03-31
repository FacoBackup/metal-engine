#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/gtc/quaternion.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "AbstractComponent.h"


namespace Metal {
    struct TransformComponent final : AbstractComponent {
        glm::mat4x4 model = glm::identity<glm::mat4x4>();
        glm::mat4x4 previousModel = glm::identity<glm::mat4x4>();
        glm::vec3 translation{};
        glm::quat rotation = glm::identity<glm::quat>();
        glm::vec3 scale{1, 1, 1};
        glm::vec3 gizmoCenter{0, 0, 0};
        bool forceTransform = false;
        bool isStatic = true;

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}

#endif //TRANSFORMCOMPONENT_H
