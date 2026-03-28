#include "TransformComponent.h"
#include "VolumeComponent.h"
#include "../../ApplicationContext.h"

#define ROTATION "Rotation"

namespace Metal {
    void TransformComponent::registerFields() {
        registerEditableField<VECTOR3>(&translation).setName("translation").setGroup("");
        registerEditableField<VECTOR3>(&scale).setName("scale").setGroup("").setMin(1);
        registerEditableField<QUAT>(&rotation).setName("rotation").setGroup("");
        registerEditableField<VECTOR3>(&gizmoCenter).setName("gizmoCenter").setGroup("");
        registerSerializableOnlyField<MAT4>(&model).setName("model");
        registerEditableField<BOOLEAN>(&isStatic).setName("isStatic").setGroup("");
    }

    ComponentType TransformComponent::getType() const {
        return TRANSFORM;
    }
}
