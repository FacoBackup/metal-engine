#include "TransformComponent.h"
#include "VolumeComponent.h"
#include "SphereLightComponent.h"
#include "PlaneLightComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../context/ApplicationContext.h"

#define ROTATION "Rotation"

namespace Metal {
    void TransformComponent::registerFields() {
        registerVec3(translation, "", "Translation");
        registerVec3(scale, "", "Scale", 1);
        registerVec3(rotationEuler, "", ROTATION);
        registerBool(isStatic, "", "Static?");
    }

    ComponentType TransformComponent::getType() {
        return TRANSFORM;
    }

    void TransformComponent::onUpdate(InspectableMember *member) {
        bool isLight = CTX.worldRepository.registry.all_of<SphereLightComponent>(entityId) ||
                       CTX.worldRepository.registry.all_of<PlaneLightComponent>(entityId);
        if (isLight) {
            CTX.engineContext.setUpdateLights(true);
        }
        if (member != nullptr && member->name == ROTATION) {
            rotation = normalize(glm::quat(rotationEuler * (glm::pi<float>() / 180.f)));
        }
        forceTransform = true;
    }

    nlohmann::json TransformComponent::toJson() const {
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

    void TransformComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        translation = {j.at("translation")[0], j.at("translation")[1], j.at("translation")[2]};
        rotation = {j.at("rotation")[3], j.at("rotation")[0], j.at("rotation")[1], j.at("rotation")[2]};
        rotationEuler = {j.at("rotationEuler")[0], j.at("rotationEuler")[1], j.at("rotationEuler")[2]};
        scale = {j.at("scale")[0], j.at("scale")[1], j.at("scale")[2]};
        gizmoCenter = {j.at("gizmoCenter")[0], j.at("gizmoCenter")[1], j.at("gizmoCenter")[2]};
        auto &m = j.at("model");
        for (int i = 0; i < 4; ++i)
            for (int k = 0; k < 4; ++k)
                model[i][k] = m[i * 4 + k];
    }
}
