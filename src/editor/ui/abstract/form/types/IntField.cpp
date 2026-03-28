#include "IntField.h"
#include <algorithm>
#include <imgui.h>
#include "common/Reflection.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    void IntField::onSync() {
        int *ptr = static_cast<int *>(field.pointer);
        if (field.disabled) {
            ImGui::Text("%s:", field.name.c_str());
            ImGui::TextDisabled("%i", *ptr);
        } else {
            ImGui::Text("%s", field.name.c_str());
            int oldValue = *ptr;
            if (ImGui::DragInt(id.c_str(), ptr, .01f, (int)field.min.value(), (int)field.max.value())) {
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

    IntField::IntField(FieldMetadata &field) : field(field) {
    }

    bool IntField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
