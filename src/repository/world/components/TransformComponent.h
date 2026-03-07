#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"


namespace Metal {
    struct TransformComponent final : AbstractComponent, Serializable {
        glm::mat4x4 model = glm::identity<glm::mat4x4>();
        glm::vec3 translation{};
        glm::vec3 rotationEuler = {0, 0, 0};
        glm::quat rotation = {0, 0, 0, 1};
        glm::vec3 scale{1, 1, 1};
        glm::vec3 gizmoCenter{0, 0, 0};
        bool forceTransform = false;
        bool isStatic = true;

        void registerFields() override;

        ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
}

#endif //TRANSFORMCOMPONENT_H
