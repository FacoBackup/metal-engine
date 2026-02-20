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
        DUMP_TEMPLATE(CTX.getAssetDirectory() + FORMAT_FILE_MATERIAL(prevSelection), *data)
        CTX.notificationService.pushMessage("Material was saved", NotificationSeverities::SUCCESS);
        if (CTX.materialService.getResources().contains(prevSelection)) {
            CTX.materialService.getResources().at(prevSelection)->dispose();
            CTX.materialService.getResources().erase(prevSelection);
        }
    }

    void MaterialInspection::onSync() {
        if (prevSelection != CTX.fileInspection.materialId) {
            delete data;
            data = CTX.materialService.stream(CTX.fileInspection.materialId);
            prevSelection = CTX.fileInspection.materialId;
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
