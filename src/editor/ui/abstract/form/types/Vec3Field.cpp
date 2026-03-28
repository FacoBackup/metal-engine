#include "Vec3Field.h"
#include <algorithm>
#include <imgui.h>
#include "common/Reflection.h"
#include "editor/ui/UIUtil.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    Vec3Field::Vec3Field(FieldMetadata &field) : field(field) {
    }

    void Vec3Field::onSync() {
        glm::vec3 *ptr = static_cast<glm::vec3 *>(field.pointer);
        if (!field.disabled) {
            values[0] = ptr->x;
            values[1] = ptr->y;
            values[2] = ptr->z;

            glm::vec3 oldValue = *ptr;
            if (UIUtil::DrawVec3Control(field.name, field.id, values, field.increment.value_or(0.01f))) {
                ptr->x = values[0];
                ptr->y = values[1];
                ptr->z = values[2];


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
            ImGui::Text("%s: <%f, %f, %f>", field.name.c_str(), ptr->x, ptr->y, ptr->z);
        }
    }

    bool Vec3Field::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
