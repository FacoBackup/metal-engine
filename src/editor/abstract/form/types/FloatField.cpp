#include "FloatField.h"
#include <algorithm>
#include <imgui.h>
#include "../../../../common/Inspectable.h"
#include "../../../service/HistoryService.h"

namespace Metal {
    void FloatField::onSync() {
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%f", field.field);
        } else {
            ImGui::Text("%s", field.name.c_str());

            float oldValue = *field.field;
            if (ImGui::DragFloat(id.c_str(), field.field, field.incrementF.value(), field.minF.value(),
                                 field.maxF.value())) {
                field.instance->registerChange();
                field.instance->onUpdate(&field);

                historyService->recordChange(field.instance, field.path, oldValue, *field.field);
            }

            if (ImGui::IsItemActivated()) {
                historyService->startTransaction("Change " + field.name);
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                historyService->endTransaction();
            }
        }
    }

    FloatField::FloatField(InspectedField<float> &field) : field(field) {
    }

    bool FloatField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
