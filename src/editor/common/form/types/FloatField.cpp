#include "FloatField.h"

namespace Metal {
    void FloatField::onSync() {
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%f", field.field);
        } else {
            ImGui::Text("%s", field.name.c_str());
            ImGui::DragFloat(id.c_str(), field.field, .01f, field.minF.value(), field.maxF.value());
        }
    }

    FloatField::FloatField(InspectedField<float> &field) : field(field) {
    }
} // Metal
