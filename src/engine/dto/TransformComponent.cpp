#include "TransformComponent.h"
#include "../../ApplicationContext.h"

#define ROTATION "Rotation"

namespace Metal {
    void TransformComponent::registerFields() {
        registerEditableField<VECTOR3>(&translation).setName("translation").setLabel("Translation");
        registerEditableField<VECTOR3>(&scale).setName("scale").setLabel("Scale").setMin(1);
        registerEditableField<QUAT>(&rotation).setName("rotation").setLabel("Rotation");
        registerEditableField<VECTOR3>(&gizmoCenter).setName("gizmoCenter").setLabel("Gizmo center");
        registerSerializableOnlyField<MAT4>(&model).setName("model");
        registerEditableField<BOOLEAN>(&isStatic).setName("isStatic").setLabel("Is static");
    }

    ComponentType TransformComponent::getType() const {
        return TRANSFORM;
    }
}
