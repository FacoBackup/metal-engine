#include "FilesHeaderPanel.h"

#include "../../../../enum/EntryType.h"
#include "../../../../dto/file/FSEntry.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../util/FilesUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    void FilesHeaderPanel::onSync() {
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesService::CreateDirectory(filesContext.currentDirectory);
            FilesService::GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Create folder");
        ImGui::SameLine();

        if (filesContext.currentDirectory->parent != nullptr) {
            if (UIUtil::ButtonSimple(Icons::arrow_upward, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                     UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                filesContext.setCurrentDirectory(filesContext.currentDirectory->parent);
            }
            UIUtil::RenderTooltip("Go to parent folder");

        }

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::refresh, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesService::GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Refresh");

        ImGui::SameLine();

        ImGui::Text(filesContext.pathToCurrentDirectory.c_str());
        UIUtil::DynamicSpacing(200);

        ImGui::SetNextItemWidth(100);
        editorMode = IndexOfValue(filesContext.filterType);
        if (ImGui::Combo((id + "filter").c_str(), &editorMode, EntryType::Names)) {
            filesContext.filterType = EntryType::ValueOfIndex(editorMode);
        }
        ImGui::SameLine();
        if (UIUtil::ButtonSimple(actionLabel + id, 100,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            action();
        }
    }
} // Metal
