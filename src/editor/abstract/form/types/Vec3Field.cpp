#include "Vec3Field.h"
#include <algorithm>
#include <imgui.h>
#include "../../../../common/Inspectable.h"
#include "../../../util/UIUtil.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    Vec3Field::Vec3Field(InspectedField<glm::vec3> &field) : field(field) {
    }

    void Vec3Field::onSync() {
        if (!field.disabled) {
            values[0] = field.field->x;
            values[1] = field.field->y;
            values[2] = field.field->z;

            glm::vec3 oldValue = *field.field;
            if (UIUtil::DrawVec3Control(field.name, field.id, values, field.incrementF.value())) {
                field.field->x = values[0];
                field.field->y = values[1];
                field.field->z = values[2];


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
            ImGui::Text("%s: <%f, %f, %f>", field.name.c_str(), field.field->x, field.field->y, field.field->z);
        }
    }

    bool Vec3Field::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
