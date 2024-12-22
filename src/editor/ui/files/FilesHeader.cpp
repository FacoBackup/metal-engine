#include "FilesHeader.h"

#include "../../../common/interface/Icons.h"
#include "../../../common/util/files/FilesUtil.h"
#include "../../../context/ApplicationContext.h"
#include "../../../common/util/UIUtil.h"

namespace Metal {
    void FilesHeader::onSync() {
        renderOptions();

        ImGui::Text(filesContext.pathToCurrentDirectory.c_str());
        UIUtil::DynamicSpacing(155);
        editorMode = EntryType::IndexOfValue(filesContext.filterType);
        if (ImGui::Combo((id + "filter").c_str(), &editorMode, EntryType::Names)) {
            filesContext.filterType = EntryType::ValueOfIndex(editorMode);
        }
        UIUtil::Spacing();
        if (UIUtil::ButtonSimple(actionLabel + id, 100,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            action();
        }
    }

    void FilesHeader::renderOptions() const {
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesUtil::MkDir(
                filesContext.currentDirectory->absolutePath + "/new-directory (" + Util::uuidV4().substr(0, 4) + ")");
            FilesService::GetEntries(filesContext.currentDirectory);
        }
        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::arrow_upward, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesContext.setCurrentDirectory(filesContext.currentDirectory->parent);
        }
        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::refresh, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesService::GetEntries(filesContext.currentDirectory);
        }
        ImGui::SameLine();
    }
} // Metal
