#include "StringField.h"

#include <imgui.h>

namespace Metal {
    StringField::StringField(InspectedField<std::string> &field) : field(field) {
    }

    void StringField::onSync() {
        if (!field.disabled) {
            strcpy(buffer, field.field->c_str());
            ImGui::Text(field.name.c_str());
            if (ImGui::InputText(field.id.c_str(), buffer, sizeof(buffer))) {
                *field.field = buffer;
            }
        } else {
            ImGui::Text("%s: %s", field.name.c_str(), field.field->c_str());
        }
    }
} // Metal
