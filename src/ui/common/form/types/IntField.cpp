#include "IntField.h"

namespace Metal {
    void IntField::onSync() {
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%i", field.field);
        } else {
            ImGui::Text("%s", field.name.c_str());
            ImGui::DragInt(id.c_str(), &field.field, .01f, field.min.value(), field.max.value());
        }
    }

    IntField::IntField(InspectedField<int> &field) : field(field) {

    }
} // Metal