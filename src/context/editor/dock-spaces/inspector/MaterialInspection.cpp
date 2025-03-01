#include "MaterialInspection.h"

#include <cereal/archives/binary.hpp>

#include "../../../ApplicationContext.h"
#include "../../../../util/UIUtil.h"
#include "../../../../util/serialization-definitions.h"
#include "../../../../dto/buffers/MaterialInfo.h"
#include "../../../../enum/engine-definitions.h"

namespace Metal {
    void MaterialInspection::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
    }

    void MaterialInspection::saveChanges() {
        DUMP_TEMPLATE(context->getAssetDirectory() + FORMAT_FILE_MATERIAL(prevSelection), *data)
        context->notificationService.pushMessage("Material was saved", NotificationSeverities::SUCCESS);
        if (context->materialService.getResources().contains(prevSelection)) {
            context->materialService.getResources().at(prevSelection)->dispose(context->vulkanContext);
            context->materialService.getResources().erase(prevSelection);
        }
        context->engineContext.updateTLASs();
    }

    void MaterialInspection::onSync() {
        if (prevSelection != context->fileInspection.materialId) {
            delete data;
            data = context->materialService.stream(context->fileInspection.materialId);
            prevSelection = context->fileInspection.materialId;
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
