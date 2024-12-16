#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

#include "../impl/AbstractComponent.h"

namespace Metal {
    struct TransformComponent final : AbstractComponent {
        glm::mat4x4 model{};
        glm::vec3 translation{};
        glm::vec4 rotation = {0, 0, 0, 1};
        glm::vec3 scale{1, 1, 1};

        const char *getIcon() override;

        const char *getTitle() override;

        void registerFields() override;
    };
}

#endif //TRANSFORMCOMPONENT_H
