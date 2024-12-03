#include "FormPanel.h"
#include "AccordionPanel.h"
#include "../../../common/inspection/FieldType.h"

namespace Metal {
    void FormPanel::processFields(std::unordered_map<std::string, AccordionPanel *> &groups) {
        for (InspectableMember &field: inspection->getFields()) {
            if (!groups.contains(field.group)) {
                const auto panel = new AccordionPanel();
                groups[field.group] = panel;
                appendChild(panel);
            }

            AccordionPanel *group = groups[field.group];
            group->title = field.group;
            if (field.isField()) {
                auto &fieldCasted = dynamic_cast<InspectedField &>(field);
//                switch (fieldCasted.type) {
//                    case STRING:
//                        // if (field.getField().isAnnotationPresent<ResourceTypeField>()) {
//                        //     group->appendChild(new ResourceField(field, changeHandler));
//                        // } else if (field.getField().isAnnotationPresent<TypePreviewField>()) {
//                        //     group->appendChild(new PreviewField(field, changeHandler));
//                        // } else {
//                        group->appendChild(new StringField(field, changeHandler));
//                        // }
//                        break;
//                    case INT:
//                        group->appendChild(new IntField(field, changeHandler));
//                        break;
//                    case FLOAT:
//                        group->appendChild(new FloatField(field, changeHandler));
//                        break;
//                    case BOOLEAN:
//                        group->appendChild(new BooleanField(field, changeHandler));
//                        break;
//                    case VECTOR2:
//                        group->appendChild(new Vector2Field(field, changeHandler));
//                        break;
//                    case VECTOR3:
//                        group->appendChild(new Vector3Field(field, changeHandler));
//                        break;
//                    case VECTOR4:
//                        group->appendChild(new Vector4Field(field, changeHandler));
//                        break;
//                    case QUATERNION:
//                        group->appendChild(new QuaternionField(field, changeHandler));
//                        break;
//                    case COLOR:
//                        group->appendChild(new ColorField(field, changeHandler));
//                        break;
//                    case OPTIONS:
//                        group->appendChild(new OptionsField(field, changeHandler));
//                        break;
//                    case LIST:
//                        break;
//                    case COMPOSITE:
//                        break;
//                    case CUSTOM:
//                        break;
//                }
            } else {
                // TODO - Method
            }
        }
    }

    void FormPanel::setInspection(Inspectable *inspection) {
        this->inspection = inspection;
        removeAllChildren();

        if (inspection == nullptr) {
            return;
        }
        std::unordered_map<std::string, AccordionPanel *> groups;
        processFields(groups);
    }

    void FormPanel::onSync() {
        if (inspection == nullptr) return;
    }
}
