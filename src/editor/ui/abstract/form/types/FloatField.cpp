#include "FloatField.h"
#include <algorithm>
#include <imgui.h>
#include "common/Reflection.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    void FloatField::onSync() {
        auto *ptr = static_cast<float *>(field.pointer);
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%f", *ptr);
        } else {
            ImGui::Text("%s", field.name.c_str());

            float oldValue = *ptr;
            if (ImGui::DragFloat(id.c_str(), ptr, field.increment.value(), field.min.value(),
                                 field.max.value())) {
                historyService->recordChange(&field, oldValue);
                ApplicationEventContext::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
            }

            if (ImGui::IsItemActivated()) {
                historyService->startTransaction("Change " + field.name);
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                historyService->endTransaction();
            }
        }
    }

    FloatField::FloatField(FieldMetadata &field) : field(field) {
    }

    bool FloatField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
