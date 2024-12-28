#include "FloatField.h"

#include <imgui.h>
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    void FloatField::onSync() {
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%f", field.field);
        } else {
            ImGui::Text("%s", field.name.c_str());
            if (ImGui::DragFloat(id.c_str(), field.field, field.incrementF.value(), field.minF.value(),
                                 field.maxF.value())) {
                field.instance->registerChange();
                field.instance->onUpdate(&field, *context);
            }
        }
    }

    FloatField::FloatField(InspectedField<float> &field) : field(field) {
    }
} // Metal
