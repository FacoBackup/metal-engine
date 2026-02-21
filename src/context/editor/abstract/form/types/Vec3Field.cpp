#include "Vec3Field.h"

#include <imgui.h>
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    Vec3Field::Vec3Field(InspectedField<glm::vec3> &field) : field(field) {
    }

    void Vec3Field::onSync() {
        if (!field.disabled) {
            values[0] = field.field->x;
            values[1] = field.field->y;
            values[2] = field.field->z;
            ImGui::Text(field.name.c_str());
            if (ImGui::DragFloat3(field.id.c_str(), values, field.incrementF.value())) {
                field.field->x = values[0];
                field.field->y = values[1];
                field.field->z = values[2];
                field.instance->registerChange();
                field.instance->onUpdate(&field);
            }
        } else {
            ImGui::Text("%s: <%f, %f, %f>", field.name.c_str(), field.field->x, field.field->y, field.field->z);
        }
    }
} // Metal
