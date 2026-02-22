#include "StringField.h"
#include <algorithm>
#include <imgui.h>
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    StringField::StringField(InspectedField<std::string> &field) : field(field) {
    }

    void StringField::onSync() {
        if (!field.disabled) {
            strcpy(buffer, field.field->c_str());
            ImGui::Text(field.name.c_str());
            if (ImGui::InputText(field.id.c_str(), buffer, sizeof(buffer))) {
                *field.field = buffer;
                field.instance->registerChange();
field.instance->onUpdate(&field);
            }
        } else {
            ImGui::Text("%s: %s", field.name.c_str(), field.field->c_str());
        }
    }

    bool StringField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
