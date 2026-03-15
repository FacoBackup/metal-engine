#include "BooleanField.h"
#include <algorithm>
#include <imgui.h>

#include "../../../../common/Inspectable.h"
#include "../../../service/HistoryService.h"

namespace Metal {
    BooleanField::BooleanField(InspectedField<bool> &field) : field(field) {
    }

    void BooleanField::onSync() {
        if (!field.disabled) {
            bool oldValue = *field.field;
            if (ImGui::Checkbox(field.nameWithId.c_str(), field.field)) {
                field.instance->registerChange();
                historyService->recordChange(&field, oldValue);
            }
        } else {
            ImGui::Text("%s: %b", field.name.c_str(), *field.field);
        }
    }

    bool BooleanField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
}
