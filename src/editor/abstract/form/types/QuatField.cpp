#include "QuatField.h"
#include <algorithm>
#include <imgui.h>
#include <glm/detail/type_quat.hpp>
#include "../../../../common/Inspectable.h"
#include "../../../util/UIUtil.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    QuatField::QuatField(InspectedField<glm::quat> &field) : field(field) {
    }

    void QuatField::onSync() {
        if (!field.disabled) {
            values[0] = field.field->x;
            values[1] = field.field->y;
            values[2] = field.field->z;
            values[3] = field.field->w;

            glm::quat oldValue = *field.field;
            if (UIUtil::DrawQuatControl(field.name, field.id, values, field.incrementF.value())) {
                field.field->x = values[0];
                field.field->y = values[1];
                field.field->z = values[2];
                field.field->w = values[3];


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
            ImGui::Text("%s: <%f, %f, %f, %f>", field.name.c_str(), field.field->x, field.field->y, field.field->z,
                        field.field->w);
        }
    }

    bool QuatField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
