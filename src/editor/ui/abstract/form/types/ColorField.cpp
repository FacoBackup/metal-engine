#include "ColorField.h"
#include <algorithm>
#include <imgui.h>
#include "common/Reflection.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    ColorField::ColorField(FieldMetadata &field) : field(field) {
    }

    void ColorField::onSync() {
        glm::vec3 *ptr = static_cast<glm::vec3 *>(field.pointer);
        if (!field.disabled) {
            values[0] = ptr->x;
            values[1] = ptr->y;
            values[2] = ptr->z;
            ImGui::Text(field.label.c_str());

            glm::vec3 oldValue = *ptr;
            if (ImGui::ColorPicker3(field.id.c_str(), values, ImGuiColorEditFlags_NoSidePreview)) {
                ptr->x = values[0];
                ptr->y = values[1];
                ptr->z = values[2];
                historyService->recordChange(&field, oldValue);
                ApplicationEventContext::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
            }

            if (ImGui::IsItemActivated()) {
                historyService->startTransaction("Change " + field.label);
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                historyService->endTransaction();
            }
        } else {
            ImGui::ColorButton("%s", ImVec4(ptr->x, ptr->y, ptr->z, 1.));
        }
    }

    bool ColorField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.label;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
