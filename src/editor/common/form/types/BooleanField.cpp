#include "BooleanField.h"

namespace Metal {
    BooleanField::BooleanField(InspectedField<bool> &field) : field(field) {
    }

    void BooleanField::onSync() {
        if (!field.disabled) {
            ImGui::Checkbox(field.name.c_str(), field.field);
        } else {
            ImGui::Text("%s: %b", field.name.c_str(), *field.field);
        }
    }
}
