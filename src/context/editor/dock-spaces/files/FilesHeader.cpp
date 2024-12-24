#include "FilesHeader.h"

#include "../../../../enum/EntryType.h"
#include "../../../../dto/file/FileEntry.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../util/FilesUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    void FilesHeader::onSync() {
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesUtil::MkDir(
                filesContext.currentDirectory->absolutePath + "/new-directory (" + Util::uuidV4().substr(0, 4) + ")");
            FilesService::GetEntries(filesContext.currentDirectory);
                                 }
        ImGui::SameLine();

        if (filesContext.currentDirectory->parent != nullptr) {
            if (UIUtil::ButtonSimple(Icons::arrow_upward, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                filesContext.setCurrentDirectory(filesContext.currentDirectory->parent);
            }
        }

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::refresh, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesService::GetEntries(filesContext.currentDirectory);
        }
        ImGui::SameLine();

        ImGui::Text(filesContext.pathToCurrentDirectory.c_str());
        UIUtil::DynamicSpacing(214);
        ImGui::SetNextItemWidth(100);
        editorMode = EntryType::IndexOfValue(filesContext.filterType);
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
