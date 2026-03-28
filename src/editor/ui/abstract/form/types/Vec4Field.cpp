#include "Vec4Field.h"
#include <algorithm>
#include <imgui.h>
#include "common/Reflection.h"
#include "editor/ui/UIUtil.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    Vec4Field::Vec4Field(FieldMetadata &field) : field(field) {
    }

    void Vec4Field::onSync() {
        glm::vec4 *ptr = static_cast<glm::vec4 *>(field.pointer);
        if (!field.disabled) {
            values[0] = ptr->x;
            values[1] = ptr->y;
            values[2] = ptr->z;
            values[3] = ptr->w;

            glm::vec4 oldValue = *ptr;
            if (UIUtil::DrawVec4Control(field.label, field.id, values, field.increment.value_or(0.01f))) {
                ptr->x = values[0];
                ptr->y = values[1];
                ptr->z = values[2];
                ptr->w = values[3];


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
            ImGui::Text("%s: <%f, %f, %f, %f>", field.label.c_str(), ptr->x, ptr->y, ptr->z,
                        ptr->w);
        }
    }

    bool Vec4Field::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.label;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
