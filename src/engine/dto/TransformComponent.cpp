#include "TransformComponent.h"
#include "VolumeComponent.h"
#include "../../ApplicationContext.h"

#define ROTATION "Rotation"

namespace Metal {
    void TransformComponent::registerFields() {
        registerEditableField(&translation, VECTOR3, "translation", "");
        registerEditableField(&scale, VECTOR3, "scale", "").setMin(1);
        registerEditableField(&rotation, QUAT, "rotation", "");
        registerEditableField(&gizmoCenter, VECTOR3, "gizmoCenter", "");
        registerSerializableOnlyField(&model, VECTOR4, "model").setTransformer(
            [this] {
                return nlohmann::json{
                    model[0][0], model[0][1], model[0][2], model[0][3],
                    model[1][0], model[1][1], model[1][2], model[1][3],
                    model[2][0], model[2][1], model[2][2], model[2][3],
                    model[3][0], model[3][1], model[3][2], model[3][3]
                };
            },
            [this](const nlohmann::json &j) {
                for (int i = 0; i < 4; ++i)
                    for (int k = 0; k < 4; ++k)
                        model[i][k] = j[i * 4 + k];
            });
        registerEditableField(&isStatic, BOOLEAN, "isStatic", "");
    }

    ComponentType TransformComponent::getType() const {
        return TRANSFORM;
    }
}
