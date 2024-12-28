#include "Vec2Field.h"

#include <imgui.h>
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    Vec2Field::Vec2Field(InspectedField<glm::vec2> &field) : field(field) {
    }

    void Vec2Field::onSync() {
        if (!field.disabled) {
            values[0] = field.field->x;
            values[1] = field.field->y;
            ImGui::Text(field.name.c_str());
            if (ImGui::DragFloat2(field.id.c_str(), values, field.incrementF.value())) {
                field.field->x = values[0];
                field.field->y = values[1];
                field.instance->registerChange();
                field.instance->onUpdate(&field, *context);
            }
        } else {
            ImGui::Text("%s: <%f, %f>", field.name.c_str(), field.field->x, field.field->y);
        }
    }
} // Metal
