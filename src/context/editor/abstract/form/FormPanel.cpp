#include "FormPanel.h"
#include "AccordionPanel.h"
#include "ChildPanel.h"
#include "../../../../enum/FieldType.h"
#include "../../../../common/inspection/Inspectable.h"
#include "../../../../context/gui/GuiContext.h"
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
        std::unordered_map<std::string, ChildPanel *> groups{};
        const auto rootPanel = new AccordionPanel();
        appendChild(rootPanel);
        rootPanel->setTitle(std::string(inspection->getIcon()) + " " + inspection->getTitle());
        for (const auto &field: inspection->getFields()) {
            if (!groups.contains(field->group)) {
                const auto panel = new ChildPanel();
                groups[field->group] = panel;
                rootPanel->appendChild(panel);
            }
            ChildPanel *group = groups[field->group];
            group->setTitle(field->group);
            switch (field->type) {
                case STRING:
                    group->appendChild(new StringField{dynamic_cast<InspectedField<std::string> &>(*field)});
                    break;
                case INT:
                    group->appendChild(new IntField{dynamic_cast<InspectedField<int> &>(*field)});
                    break;
                case FLOAT:
                    group->appendChild(new FloatField{dynamic_cast<InspectedField<float> &>(*field)});
                    break;
                case BOOLEAN:
                    group->appendChild(new BooleanField{dynamic_cast<InspectedField<bool> &>(*field)});
                    break;
                case METHOD:
                    group->appendChild(new MethodField{dynamic_cast<InspectedMethod &>(*field)});
                default:
                    break;
                case VECTOR2:
                    group->appendChild(new Vec2Field{dynamic_cast<InspectedField<glm::vec2> &>(*field)});
                    break;
                case VECTOR3:
                    group->appendChild(new Vec3Field{dynamic_cast<InspectedField<glm::vec3> &>(*field)});
                    break;
                case VECTOR4:
                    group->appendChild(new Vec4Field{dynamic_cast<InspectedField<glm::vec4> &>(*field)});
                    break;
                case QUAT:
                    group->appendChild(new QuatField{dynamic_cast<InspectedField<glm::quat> &>(*field)});
                    break;
                case COLOR:
                    group->appendChild(new ColorField{dynamic_cast<InspectedField<glm::vec3> &>(*field)});
                    break;
                case RESOURCE:
                    group->appendChild(new ResourceField{dynamic_cast<InspectedField<std::string> &>(*field)});
                    break;
                    //                    case OPTIONS:
                    //                        group->appendChild(new OptionsField(field, changeHandler));
                    //                        break;
                    //                    case LIST:
                    //                        break;
                    //                    case COMPOSITE:
                    //                        break;
                    //                    case CUSTOM:
                    //                        break;
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
        onSyncChildren();
    }

    void FormPanel::resetForm() {
        removeAllChildren();
        inspectionMap.clear();
    }
}
