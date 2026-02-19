#include "BooleanField.h"

#include <imgui.h>

#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    BooleanField::BooleanField(InspectedField<bool> &field) : field(field) {
    }

    void BooleanField::onSync() {
        if (!field.disabled) {
            if(ImGui::Checkbox(field.nameWithId.c_str(), field.field)){
                field.instance->registerChange();
                field.instance->onUpdate(&field);
            }
        } else {
            ImGui::Text("%s: %b", field.name.c_str(), *field.field);
        }
    }
}
