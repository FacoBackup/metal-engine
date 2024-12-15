#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

#include "AbstractComponent.h"
#include "../../../../common/interface/Icons.h"

namespace Metal {
    struct TransformComponent final : AbstractComponent {
        glm::mat4x4 model{};
        glm::vec3 translation{};
        glm::vec4 rotation = {0, 0, 0, 1};
        glm::vec3 scale{};

        const char *getIcon() override {
            return Icons::transform.c_str();
        }

        const char *getTitle() override {
            return "Transformation Component";
        }
    };
}

#endif //TRANSFORMCOMPONENT_H
