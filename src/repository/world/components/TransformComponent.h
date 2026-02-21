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

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entityId"] = entityId;
            j["translation"] = {translation.x, translation.y, translation.z};
            j["rotation"] = {rotation.x, rotation.y, rotation.z, rotation.w};
            j["rotationEuler"] = {rotationEuler.x, rotationEuler.y, rotationEuler.z};
            j["scale"] = {scale.x, scale.y, scale.z};
            j["gizmoCenter"] = {gizmoCenter.x, gizmoCenter.y, gizmoCenter.z};
            j["model"] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]
            };
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            entityId = j.at("entityId").get<EntityID>();
            translation = {j.at("translation")[0], j.at("translation")[1], j.at("translation")[2]};
            rotation = {j.at("rotation")[3], j.at("rotation")[0], j.at("rotation")[1], j.at("rotation")[2]};
            // Note: glm::quat(w, x, y, z) or {x, y, z, w}?
            rotationEuler = {j.at("rotationEuler")[0], j.at("rotationEuler")[1], j.at("rotationEuler")[2]};
            scale = {j.at("scale")[0], j.at("scale")[1], j.at("scale")[2]};
            gizmoCenter = {j.at("gizmoCenter")[0], j.at("gizmoCenter")[1], j.at("gizmoCenter")[2]};
            auto &m = j.at("model");
            for (int i = 0; i < 4; ++i)
                for (int k = 0; k < 4; ++k)
                    model[i][k] = m[i * 4 + k];
        }
    };
}

#endif //TRANSFORMCOMPONENT_H
