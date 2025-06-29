#include "FilesHeader.h"

#include "../../../../common/enum/EntryType.h"
#include "../../../../common/FileEntry.h"
#include "../../../../common/enum/Icons.h"
#include "../../../../common/FilesUtil.h"
#include "../../../../MetalContextProvider.h"
#include "../../../util/UIUtil.h"

namespace Metal {
    void FilesHeader::onSync() {
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesUtil::MkDir(
                filesContext.currentDirectory->absolutePath + "/new-directory (" + Util::uuidV4().substr(0, 4) + ")");
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
        UIUtil::DynamicSpacing(264);

        if (UIUtil::ButtonSimple(Icons::format_paint + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            SINGLETONS.filesService.createMaterial(filesContext.currentDirectory->absolutePath);
            FilesService::GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Create material");
        ImGui::SameLine();

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
