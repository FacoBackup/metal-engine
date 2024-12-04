#include "BooleanField.h"

namespace Metal {
    BooleanField::BooleanField(InspectedField<bool> &field) : field(field) {
    }

    void BooleanField::onSync() {
        if (ImGui::Checkbox(field.name.c_str(), &field.field)) {
            if (!field.disabled) {
                field.field = !field.field;
            }
        }
    }
}