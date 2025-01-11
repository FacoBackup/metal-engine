#include "MaterialInspection.h"

#include <cereal/archives/binary.hpp>

#include "../../../ApplicationContext.h"
#include "../../../../util/UIUtil.h"
#include "../../../../util/serialization-definitions.h"
#include "../../../../service/material/MaterialData.h"
#include "../../../../enum/engine-definitions.h"

namespace Metal {
    void MaterialInspection::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
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
        ImGui::Separator();
        if (UIUtil::ButtonSimple(Icons::save + "Save changes" + id, std::max(ImGui::GetContentRegionAvail().x / 2.f, 100.f), UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            DUMP_TEMPLATE(context->getAssetDirectory() + FORMAT_FILE_MATERIAL(prevSelection), *data)
            context->notificationService.pushMessage("Material was saved", NotificationSeverities::SUCCESS);
            context->materialService.getResources().at(prevSelection)->dispose(context->vulkanContext);
            context->materialService.getResources().erase(prevSelection);
        }
        UIUtil::RenderTooltip("Save changes");
        ImGui::Separator();
        ImGui::Spacing();

        formPanel->setInspection(data);
        formPanel->onSync();
    }
} // Metal
