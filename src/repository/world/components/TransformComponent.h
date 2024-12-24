#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../impl/AbstractComponent.h"

namespace Metal {
    struct TransformComponent final : AbstractComponent {
        glm::mat4x4 model = glm::identity<glm::mat4x4>();
        glm::vec3 translation{};
        glm::quat rotation = {0, 0, 0, 1};
        glm::vec3 scale{1, 1, 1};
        bool forceTransform = false;
        bool isStatic = false;

        explicit TransformComponent(const EntityID &entityId)
            : AbstractComponent(entityId) {
        }

        const char *getIcon() override;

        const char *getTitle() override;

        void registerFields() override;
    };
}

#endif //TRANSFORMCOMPONENT_H
