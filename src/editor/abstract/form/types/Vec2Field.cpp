#include "Vec2Field.h"
#include <algorithm>
#include <imgui.h>
#include "../../../../common/Inspectable.h"
#include "../../../util/UIUtil.h"
#include "../../../service/HistoryService.h"

namespace Metal {
    Vec2Field::Vec2Field(InspectedField<glm::vec2> &field) : field(field) {
    }

    void Vec2Field::onSync() {
        if (!field.disabled) {
            values[0] = field.field->x;
            values[1] = field.field->y;

            glm::vec2 oldValue = *field.field;
            if (UIUtil::DrawVec2Control(field.name, field.id, values, field.incrementF.value())) {
                field.field->x = values[0];
                field.field->y = values[1];
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
        } else {
            ImGui::Text("%s: <%f, %f>", field.name.c_str(), field.field->x, field.field->y);
        }
    }

    bool Vec2Field::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
