#include "FormPanel.h"

#include <imgui.h>

#include "AccordionPanel.h"
#include "common/FieldType.h"
#include "common/Reflection.h"
#include "editor/service/HistoryService.h"
#include "editor/service/ThemeService.h"
#include "types/IntField.h"
#include "types/FloatField.h"
#include "types/BooleanField.h"
#include "types/ColorField.h"
#include "types/QuatField.h"
#include "types/ResourceField.h"
#include "editor/dto/FSEntry.h"
#include "types/StringField.h"
#include "types/Vec2Field.h"
#include "types/Vec3Field.h"
#include "types/Vec4Field.h"

namespace Metal {
    void FormPanel::processFields(Reflection *inspection) {
        if (inspection->getTitle() == nullptr) return;
        auto rootPanel = initializePanel<AccordionPanel>();
        rootPanel->setFilter(&searchFilter);
        rootPanel->setTitle(std::string(inspection->getIcon()) + " " + inspection->getTitle());
        rootPanel->setBackgroundColor(&themeService->palette3);
        for (const auto &field: inspection->getFields(UI_VISIBLE)) {

            std::shared_ptr<AbstractFormFieldPanel> fieldPanel = nullptr;
            switch (field->type) {
                case STRING:
                    fieldPanel = std::make_shared<StringField>(*field);
                    break;
                case INT:
                    fieldPanel = std::make_shared<IntField>(*field);
                    break;
                case FLOAT:
                    fieldPanel = std::make_shared<FloatField>(*field);
                    break;
                case BOOLEAN:
                    fieldPanel = std::make_shared<BooleanField>(*field);
                    break;
                case VECTOR2:
                    fieldPanel = std::make_shared<Vec2Field>(*field);
                    break;
                case VECTOR3:
                    fieldPanel = std::make_shared<Vec3Field>(*field);
                    break;
                case VECTOR4:
                    fieldPanel = std::make_shared<Vec4Field>(*field);
                    break;
                case QUAT:
                    fieldPanel = std::make_shared<QuatField>(*field);
                    break;
                case COLOR:
                    fieldPanel = std::make_shared<ColorField>(*field);
                    break;
                case RESOURCE:
                    fieldPanel = std::make_shared<ResourceField>(*field);
                    break;
                default:
                    break;
            }
            if (fieldPanel) {
                fieldPanel->setFilter(&searchFilter);
                rootPanel->initializePanel(fieldPanel);
            }
        }
    }

    void FormPanel::setInspection(Reflection *inspection) {
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
