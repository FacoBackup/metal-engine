#include "MaterialInspection.h"

#include <cereal/archives/binary.hpp>

#include "../../../ApplicationContext.h"
#include "../../../../util/UIUtil.h"
#include "../../../../util/serialization-definitions.h"
#include "../../../../service/material/MaterialFileData.h"
#include "../../../../enum/engine-definitions.h"

namespace Metal {
    void MaterialInspection::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
    }

    void MaterialInspection::saveChanges() {
        DUMP_TEMPLATE(ApplicationContext::Get().getAssetDirectory() + FORMAT_FILE_MATERIAL(prevSelection), *data)
        ApplicationContext::Get().notificationService.pushMessage("Material was saved", NotificationSeverities::SUCCESS);
        if (ApplicationContext::Get().materialService.getResources().contains(prevSelection)) {
            ApplicationContext::Get().materialService.getResources().at(prevSelection)->dispose();
            ApplicationContext::Get().materialService.getResources().erase(prevSelection);
        }
    }

    void MaterialInspection::onSync() {
        if (prevSelection != ApplicationContext::Get().fileInspection.materialId) {
            delete data;
            data = ApplicationContext::Get().materialService.stream(ApplicationContext::Get().fileInspection.materialId);
            prevSelection = ApplicationContext::Get().fileInspection.materialId;
        }
        if (prevSelection.empty()) {
            return;
        }
        ImGui::Spacing();

        formPanel->setInspection(data);
        formPanel->onSync();

        if(data->isNotFrozen()) {
            saveChanges();
            data->freezeVersion();
        }
    }
} // Metal
