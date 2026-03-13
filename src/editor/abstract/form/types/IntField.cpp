#include "IntField.h"
#include <algorithm>
#include <imgui.h>
#include "../../../../common/Inspectable.h"

namespace Metal {
    void IntField::onSync() {
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%i", field.field);
        } else {
            ImGui::Text("%s", field.name.c_str());
            if (ImGui::DragInt(id.c_str(), field.field, .01f, field.min.value(), field.max.value())) {
                field.instance->registerChange();
                field.instance->onUpdate(&field);
            }
        }
    }

    IntField::IntField(InspectedField<int> &field) : field(field) {
    }

    bool IntField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
