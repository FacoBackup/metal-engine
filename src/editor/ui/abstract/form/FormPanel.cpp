#include "FormPanel.h"

#include <imgui.h>

#include "AccordionPanel.h"
#include "common/FieldType.h"
#include "common/Inspectable.h"
#include "editor/service/HistoryService.h"
#include "editor/service/ThemeService.h"
#include "types/IntField.h"
#include "types/FloatField.h"
#include "types/BooleanField.h"
#include "types/ColorField.h"
#include "types/MethodField.h"
#include "types/QuatField.h"
#include "types/ResourceField.h"
#include "editor/dto/FSEntry.h"
#include "types/StringField.h"
#include "types/Vec2Field.h"
#include "types/Vec3Field.h"
#include "types/Vec4Field.h"

namespace Metal {
    void FormPanel::processFields(Inspectable *inspection) {
        std::unordered_map<std::string, std::shared_ptr<AccordionPanel>> groups{};
        auto rootPanel = initializePanel<AccordionPanel>();
        rootPanel->setFilter(&searchFilter);
        rootPanel->setTitle(std::string(inspection->getIcon()) + " " + inspection->getTitle());
        rootPanel->setBackgroundColor(themeService->palette3);
        for (const auto &field: inspection->getFields()) {
            if (!groups.contains(field->group)) {
                auto panel = rootPanel->initializePanel<AccordionPanel>();
                panel->setFilter(&searchFilter);
                panel->setBackgroundColor(themeService->palette4);
                groups[field->group] = panel;
            }
            std::shared_ptr<AccordionPanel> group = groups[field->group];
            group->setTitle(field->group);
            std::shared_ptr<AbstractFormFieldPanel> fieldPanel = nullptr;
            switch (field->type) {
                case STRING:
                    fieldPanel = std::make_shared<StringField>(dynamic_cast<InspectedField<std::string> &>(*field));
                    break;
                case INT:
                    fieldPanel = std::make_shared<IntField>(dynamic_cast<InspectedField<int> &>(*field));
                    break;
                case FLOAT:
                    fieldPanel = std::make_shared<FloatField>(dynamic_cast<InspectedField<float> &>(*field));
                    break;
                case BOOLEAN:
                    fieldPanel = std::make_shared<BooleanField>(dynamic_cast<InspectedField<bool> &>(*field));
                    break;
                case METHOD:
                    fieldPanel = std::make_shared<MethodField>(dynamic_cast<InspectedMethod &>(*field));
                    break;
                case VECTOR2:
                    fieldPanel = std::make_shared<Vec2Field>(dynamic_cast<InspectedField<glm::vec2> &>(*field));
                    break;
                case VECTOR3:
                    fieldPanel = std::make_shared<Vec3Field>(dynamic_cast<InspectedField<glm::vec3> &>(*field));
                    break;
                case VECTOR4:
                    fieldPanel = std::make_shared<Vec4Field>(dynamic_cast<InspectedField<glm::vec4> &>(*field));
                    break;
                case QUAT:
                    fieldPanel = std::make_shared<QuatField>(dynamic_cast<InspectedField<glm::quat> &>(*field));
                    break;
                case COLOR:
                    fieldPanel = std::make_shared<ColorField>(dynamic_cast<InspectedField<glm::vec3> &>(*field));
                    break;
                case RESOURCE:
                    fieldPanel = std::make_shared<ResourceField>(dynamic_cast<InspectedField<std::string> &>(*field));
                    break;
                default:
                    break;
            }
            if (fieldPanel) {
                fieldPanel->setFilter(&searchFilter);
                group->initializePanel(fieldPanel);
            }
        }
    }

    void FormPanel::setInspection(Inspectable *inspection) {
        if (inspection == nullptr) {
            return;
        }
        if (!inspectionMap.contains(inspection->getUniqueId())) {
            inspectionMap.emplace(inspection->getUniqueId(), inspection);

            processFields(inspection);
        }
    }

    void FormPanel::onSync() {
        if (ImGui::InputTextWithHint((id + "Search").c_str(), "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer))) {
            searchFilter = searchBuffer;
            std::transform(searchFilter.begin(), searchFilter.end(), searchFilter.begin(), ::tolower);
        }
        for (const auto &panel: children) {
            if (std::dynamic_pointer_cast<AbstractFormFieldPanel>(panel)->isVisible()) {
                panel->onSync();
            }
        }
    }

    void FormPanel::resetForm() {
        removeAllChildren();
        inspectionMap.clear();
    }
}
