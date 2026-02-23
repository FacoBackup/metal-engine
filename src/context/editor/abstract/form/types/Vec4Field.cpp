#include "Vec4Field.h"
#include <algorithm>
#include <imgui.h>
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    Vec4Field::Vec4Field(InspectedField<glm::vec4> &field) : field(field) {
    }

    void Vec4Field::onSync() {
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

    bool Vec4Field::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
