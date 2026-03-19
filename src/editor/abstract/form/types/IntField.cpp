#include "IntField.h"
#include <algorithm>
#include <imgui.h>
#include "../../../../common/Inspectable.h"
#include "editor/service/HistoryService.h"
#include "editor/service/EventService.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    void IntField::onSync() {
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%i", field.field);
        } else {
            ImGui::Text("%s", field.name.c_str());
            int oldValue = *field.field;
            if (ImGui::DragInt(id.c_str(), field.field, .01f, field.min.value(), field.max.value())) {
                field.instance->registerChange();
                historyService->recordChange(&field, oldValue);
                EventService::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
            }

            if (ImGui::IsItemActivated()) {
                historyService->startTransaction("Change " + field.name);
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                historyService->endTransaction();
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
