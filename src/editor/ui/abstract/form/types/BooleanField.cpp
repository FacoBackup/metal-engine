#include "BooleanField.h"
#include <algorithm>
#include <imgui.h>

#include "common/Reflection.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    BooleanField::BooleanField(FieldMetadata &field) : field(field) {
    }

    void BooleanField::onSync() {
        if (!field.disabled) {
            bool *ptr = static_cast<bool *>(field.pointer);
            bool oldValue = *ptr;
            if (ImGui::Checkbox(field.labelWithId.c_str(), ptr)) {
                historyService->recordChange(&field, oldValue);
                ApplicationEventContext::dispatch(field.instance->getClassName(),
                                                  std::make_shared<FieldModificationPayload>(field));
            }
        } else {
            ImGui::Text("%s: %b", field.label.c_str(), *static_cast<bool *>(field.pointer));
        }
    }

    bool BooleanField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.label;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
}
