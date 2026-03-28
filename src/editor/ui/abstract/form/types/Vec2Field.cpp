#include "Vec2Field.h"
#include <algorithm>
#include <imgui.h>
#include "common/Reflection.h"
#include "editor/ui/UIUtil.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    Vec2Field::Vec2Field(FieldMetadata &field) : field(field) {
    }

    void Vec2Field::onSync() {
        glm::vec2 *ptr = static_cast<glm::vec2 *>(field.pointer);
        if (!field.disabled) {
            values[0] = ptr->x;
            values[1] = ptr->y;

            glm::vec2 oldValue = *ptr;
            if (UIUtil::DrawVec2Control(field.name, field.id, values, field.increment.value_or(0.01f))) {
                ptr->x = values[0];
                ptr->y = values[1];

                historyService->recordChange(&field, oldValue);
                ApplicationEventContext::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
            }

            if (ImGui::IsItemActivated()) {
                historyService->startTransaction("Change " + field.name);
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                historyService->endTransaction();
            }
        } else {
            ImGui::Text("%s: <%f, %f>", field.name.c_str(), ptr->x, ptr->y);
        }
    }

    bool Vec2Field::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
