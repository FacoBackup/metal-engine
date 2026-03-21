#include "FilesHeaderPanel.h"
#include <filesystem>

#include "../../dto/FSEntry.h"
#include "../../../common/Icons.h"
#include "../../service/FilesService.h"
#include "../../util/UIUtil.h"

#include <cstring>

namespace Metal {
    void FilesHeaderPanel::onSync() {
        if (addressBuffer[0] == '\0' || filesContext.currentDirectory->absolutePath != addressBuffer) {
            std::strncpy(addressBuffer, filesContext.currentDirectory->absolutePath.c_str(), sizeof(addressBuffer));
        }

        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        if (UIUtil::ButtonSimple(Icons::arrow_upward, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            std::filesystem::path currentPath(filesContext.currentDirectory->absolutePath);
            if (currentPath.has_parent_path()) {
                std::string parentPath = currentPath.parent_path().string();
                std::shared_ptr<FSEntry> parentEntry = filesService->GetEntry(parentPath);
                if (parentEntry) {
                    filesContext.setCurrentDirectory(parentEntry);
                    filesService->GetEntries(parentEntry);
                }
            }
        }
        UIUtil::RenderTooltip("Go to parent folder");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple((filesContext.viewMode == FilesViewMode::LIST ? Icons::grid_view : Icons::view_list) + id,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesContext.viewMode = (filesContext.viewMode == FilesViewMode::LIST) ? FilesViewMode::CARD : FilesViewMode::LIST;
        }
        UIUtil::RenderTooltip(filesContext.viewMode == FilesViewMode::LIST ? "Switch to Card view" : "Switch to List view");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesService->CreateDirectory(filesContext.currentDirectory);
            filesService->GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Create folder");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::refresh, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesService->GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Refresh");

        ImGui::PopStyleVar();
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 220.0f);
        if (ImGui::InputText("##AddressBar", addressBuffer, sizeof(addressBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::shared_ptr<FSEntry> newDir = filesService->GetEntry(addressBuffer);
            if (newDir && newDir->isDirectory) {
                filesContext.setCurrentDirectory(newDir);
                filesService->GetEntries(newDir);
            } else {
                std::strncpy(addressBuffer, filesContext.currentDirectory->absolutePath.c_str(), sizeof(addressBuffer));
            }
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputTextWithHint("##Search", (Icons::search + " Search...").c_str(), filesContext.searchQuery, sizeof(filesContext.searchQuery));
    }
} // Metal
