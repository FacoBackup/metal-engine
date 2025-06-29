#include "MaterialInspection.h"

#include <cereal/archives/binary.hpp>

#include "../../../../MetalContextProvider.h"
#include "../../../util/UIUtil.h"
#include "../../../../common/serialization-definitions.h"
#include "../../../../renderer/data/MaterialInfo.h"
#include "../../../../common/engine-definitions.h"

namespace Metal {
    void MaterialInspection::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
    }

    void MaterialInspection::saveChanges() {
        DUMP_TEMPLATE(ContextProvider::Get().getAssetDirectory() + FORMAT_FILE_MATERIAL(prevSelection), *data)
        SINGLETONS.notificationService.pushMessage("Material was saved", NotificationSeverities::SUCCESS);
        if (SINGLETONS.materialService.getResources().contains(prevSelection)) {
            SINGLETONS.materialService.getResources().at(prevSelection)->dispose();
            SINGLETONS.materialService.getResources().erase(prevSelection);
        }
        SINGLETONS.engineContext.dispatchTLASUpdate();
    }

    void MaterialInspection::onSync() {

        if (prevSelection != SINGLETONS.fileInspection.materialId) {
            delete data;
            data = SINGLETONS.materialService.stream(SINGLETONS.fileInspection.materialId);
            prevSelection = SINGLETONS.fileInspection.materialId;
        }
        if (prevSelection.empty()) {
            return;
        }
        ImGui::Spacing();

        formPanel->setInspection(data);
        formPanel->onSync();

        if (data->isNotFrozen()) {
            sinceLastChange = SINGLETONS.engineContext.currentTimeMs;
            data->freezeVersion();
        }
        if(sinceLastChange != 0 && (SINGLETONS.engineContext.currentTimeMs - sinceLastChange) >= 200) {
            sinceLastChange = 0;
            saveChanges();
        }
    }
} // Metal
