#include "MaterialEditPanel.h"

#include "../../../ApplicationContext.h"
#include "../../../../util/UIUtil.h"
#include "../../../../util/serialization-definitions.h"
#include "../../../../service/material/MaterialFileData.h"
#include "../../../../enum/engine-definitions.h"

namespace Metal {
    void MaterialEditPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
    }

    void MaterialEditPanel::saveChanges() {
        CTX.engineContext.setGISettingsUpdated(true);
        data->freezeVersion();
        DUMP_TEMPLATE(CTX.getAssetDirectory() + FORMAT_FILE_MATERIAL(prevSelection), *data)
        CTX.notificationService.pushMessage("Material was saved", NotificationSeverities::SUCCESS);
        if (CTX.materialService.getResources().contains(prevSelection)) {
            CTX.materialService.getResources().at(prevSelection)->dispose();
            CTX.materialService.getResources().erase(prevSelection);
        }
    }

    void MaterialEditPanel::onSync() {
        if (prevSelection != CTX.fileInspection.materialId) {
            delete data;
            data = CTX.materialService.stream(CTX.fileInspection.materialId);
            prevSelection = CTX.fileInspection.materialId;
        }
        if (prevSelection.empty()) {
            return;
        }
        ImGui::Spacing();
        bool changed = data->isNotFrozen();
        if (changed) {
            ImGui::PushStyleColor(ImGuiCol_Button, CTX.editorRepository.accent); // Orange-ish
        }
        if (ImGui::Button(("Save" + id + "save1").c_str())) {
            saveChanges();
        }
        if (changed) {
            ImGui::PopStyleColor();
        }
        ImGui::Spacing();

        formPanel->setInspection(data);
        formPanel->onSync();

        ImGui::Spacing();
        changed = data->isNotFrozen();
        if (changed) {
            ImGui::PushStyleColor(ImGuiCol_Button, CTX.editorRepository.accent); // Orange-ish
        }
        if (ImGui::Button(("Save" + id + "save").c_str())) {
            saveChanges();
        }
        if (changed) {
            ImGui::PopStyleColor();
        }
    }
} // Metal
