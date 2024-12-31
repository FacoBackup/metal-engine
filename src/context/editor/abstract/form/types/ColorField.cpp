#include "ColorField.h"

#include <imgui.h>
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    ColorField::ColorField(InspectedField<glm::vec3> &field) : field(field) {
    }

    void ColorField::onSync() {
        if (!field.disabled) {
            values[0] = field.field->x;
            values[1] = field.field->y;
            values[2] = field.field->z;
            ImGui::Text(field.name.c_str());
            if (ImGui::ColorPicker3(field.id.c_str(), values, ImGuiColorEditFlags_NoSidePreview)) {
                field.field->x = values[0];
                field.field->y = values[1];
                field.field->z = values[2];
                field.instance->registerChange();
                field.instance->onUpdate(&field, *context);
            }
        } else {
            ImGui::ColorButton("%s", ImVec4(field.field->x, field.field->y, field.field->z, 1.));
        }
    }
} // Metal
