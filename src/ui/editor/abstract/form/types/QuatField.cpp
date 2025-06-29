#include "QuatField.h"

#include <imgui.h>
#include <glm/detail/type_quat.hpp>
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    QuatField::QuatField(InspectedField<glm::quat> &field) : field(field) {
    }

    void QuatField::onSync() {
        if (!field.disabled) {
            values[0] = field.field->x;
            values[1] = field.field->y;
            values[2] = field.field->z;
            values[3] = field.field->w;
            ImGui::Text(field.name.c_str());
            if (ImGui::DragFloat4(field.id.c_str(), values, field.incrementF.value())) {
                field.field->x = values[0];
                field.field->y = values[1];
                field.field->z = values[2];
                field.field->w = values[3];
                field.instance->registerChange();
                field.instance->onUpdate(&field);
            }
        } else {
            ImGui::Text("%s: <%f, %f, %f, %f>", field.name.c_str(), field.field->x, field.field->y, field.field->z,
                        field.field->w);
        }
    }
} // Metal
