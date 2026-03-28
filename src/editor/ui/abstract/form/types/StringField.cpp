#include "StringField.h"
#include <algorithm>
#include <imgui.h>
#include "common/Reflection.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    StringField::StringField(FieldMetadata &field) : field(field) {
    }

    void StringField::onSync() {
        std::string *ptr = static_cast<std::string *>(field.pointer);
        if (!field.disabled) {
            strcpy_s(buffer, sizeof(buffer), ptr->c_str());
            ImGui::Text(field.name.c_str());
            std::string oldValue = *ptr;
            if (ImGui::InputText(field.id.c_str(), buffer, sizeof(buffer))) {
                *ptr = buffer;
                historyService->recordChange(&field, oldValue);
                ApplicationEventContext::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
            }
        } else {
            ImGui::Text("%s: %s", field.name.c_str(), ptr->c_str());
        }
    }

    bool StringField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
