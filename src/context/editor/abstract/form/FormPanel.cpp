#include "FormPanel.h"
#include "AccordionPanel.h"
#include "ChildPanel.h"
#include "../../../../enum/FieldType.h"
#include "../../../../common/inspection/Inspectable.h"
#include "types/IntField.h"
#include "types/FloatField.h"
#include "types/BooleanField.h"
#include "types/ColorField.h"
#include "types/MethodField.h"
#include "types/QuatField.h"
#include "types/ResourceField.h"
#include "types/StringField.h"
#include "types/Vec2Field.h"
#include "types/Vec3Field.h"
#include "types/Vec4Field.h"

namespace Metal {
    void FormPanel::processFields(Inspectable *inspection) {
        std::unordered_map<std::string, AccordionPanel *> groups{};
        const auto rootPanel = new ChildPanel();
        rootPanel->setFilter(&searchFilter);
        appendChild(rootPanel);
        rootPanel->setTitle(std::string(inspection->getIcon()) + " " + inspection->getTitle());
        for (const auto &field: inspection->getFields()) {
            if (!groups.contains(field->group)) {
                const auto panel = new AccordionPanel();
                panel->setFilter(&searchFilter);
                groups[field->group] = panel;
                rootPanel->appendChild(panel);
            }
            AccordionPanel *group = groups[field->group];
            group->setTitle(field->group);
            AbstractFormFieldPanel *fieldPanel = nullptr;
            switch (field->type) {
                case STRING:
                    fieldPanel = new StringField{dynamic_cast<InspectedField<std::string> &>(*field)};
                    break;
                case INT:
                    fieldPanel = new IntField{dynamic_cast<InspectedField<int> &>(*field)};
                    break;
                case FLOAT:
                    fieldPanel = new FloatField{dynamic_cast<InspectedField<float> &>(*field)};
                    break;
                case BOOLEAN:
                    fieldPanel = new BooleanField{dynamic_cast<InspectedField<bool> &>(*field)};
                    break;
                case METHOD:
                    fieldPanel = new MethodField{dynamic_cast<InspectedMethod &>(*field)};
                    break;
                case VECTOR2:
                    fieldPanel = new Vec2Field{dynamic_cast<InspectedField<glm::vec2> &>(*field)};
                    break;
                case VECTOR3:
                    fieldPanel = new Vec3Field{dynamic_cast<InspectedField<glm::vec3> &>(*field)};
                    break;
                case VECTOR4:
                    fieldPanel = new Vec4Field{dynamic_cast<InspectedField<glm::vec4> &>(*field)};
                    break;
                case QUAT:
                    fieldPanel = new QuatField{dynamic_cast<InspectedField<glm::quat> &>(*field)};
                    break;
                case COLOR:
                    fieldPanel = new ColorField{dynamic_cast<InspectedField<glm::vec3> &>(*field)};
                    break;
                case RESOURCE:
                    fieldPanel = new ResourceField{dynamic_cast<InspectedField<std::string> &>(*field)};
                    break;
                default:
                    break;
            }
            if (fieldPanel) {
                fieldPanel->setFilter(&searchFilter);
                group->appendChild(fieldPanel);
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
        for (const auto panel: children) {
            if (dynamic_cast<AbstractFormFieldPanel *>(panel)->isVisible()) {
                panel->onSync();
            }
        }
    }

    void FormPanel::resetForm() {
        removeAllChildren();
        inspectionMap.clear();
    }
}
