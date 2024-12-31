#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"

namespace Metal {
    struct TransformComponent final : AbstractComponent {
        glm::mat4x4 model = glm::identity<glm::mat4x4>();
        glm::vec3 translation{};
        glm::vec3 rotationEuler = {0, 0, 0};
        glm::quat rotation = {0, 0, 0, 1};
        glm::vec3 scale{1, 1, 1};
        bool forceTransform = false;
        bool isStatic = true;

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member, ApplicationContext &context) override;

        SERIALIZE_TEMPLATE(
            model[0][0], model[0][1], model[0][2], model[0][3],
            model[1][0], model[1][1], model[1][2], model[1][3],
            model[2][0], model[2][1], model[2][2], model[2][3],
            model[3][0], model[3][1], model[3][2], model[3][3],
            translation.x, translation.y, translation.z,
            rotation.x, rotation.y, rotation.z, rotation.w,
            rotationEuler.x, rotationEuler.y, rotationEuler.z,
            scale.x, scale.y, scale.z,
            entityId
        )
    };
}

#endif //TRANSFORMCOMPONENT_H
