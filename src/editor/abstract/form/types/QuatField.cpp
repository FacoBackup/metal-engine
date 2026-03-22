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
            if (!isEditing) {
                eulerValues = glm::degrees(glm::eulerAngles(*field.field));
            }

            values[0] = eulerValues.x;
            values[1] = eulerValues.y;
            values[2] = eulerValues.z;

            glm::quat oldValue = *field.field;
            if (UIUtil::DrawVec3Control(field.name, field.id, values, field.incrementF.value())) {
                isEditing = true;
                eulerValues.x = values[0];
                eulerValues.y = values[1];
                eulerValues.z = values[2];

                *field.field = glm::quat(glm::radians(eulerValues));

                historyService->recordChange(&field, oldValue);
                ApplicationEventContext::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
            }

            if (ImGui::IsItemActivated()) {
                isEditing = true;
                historyService->startTransaction("Change " + field.name);
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                isEditing = false;
                historyService->endTransaction();
            }
        } else {
            glm::vec3 euler = glm::degrees(glm::eulerAngles(*field.field));
            ImGui::Text("%s: <%f, %f, %f>", field.name.c_str(), euler.x, euler.y, euler.z);
        }
    }

    bool QuatField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
