#include "FilesHeaderPanel.h"
#include <filesystem>

#include "editor/dto/FSEntry.h"
#include "common/Icons.h"
#include "../../service/FilesService.h"
#include "core/DirectoryService.h"
#include "editor/ui/UIUtil.h"
#include "common/FileExtensions.h"

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
                    filesContext.currentDirectory = parentEntry;
                    filesService->GetEntries(parentEntry);
                }
            }
        }
        UIUtil::RenderTooltip("Go to parent folder");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple((Icons::home) + id,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            std::string parentPath = directoryService->getRootDirectory();
            if (std::shared_ptr<FSEntry> parentEntry = filesService->GetEntry(parentPath)) {
                filesContext.currentDirectory = parentEntry;
                filesService->GetEntries(parentEntry);
            }
        }
        UIUtil::RenderTooltip("Go to home directory");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(
            (filesContext.viewMode == FilesViewMode::LIST ? Icons::grid_view : Icons::view_list) + id,
            UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesContext.viewMode = (filesContext.viewMode == FilesViewMode::LIST)
                                        ? FilesViewMode::CARD
                                        : FilesViewMode::LIST;
        }
        UIUtil::RenderTooltip(filesContext.viewMode == FilesViewMode::LIST
                                  ? "Switch to Card view"
                                  : "Switch to List view");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesService->CreateDirectory(filesContext.currentDirectory);
            filesService->GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Create folder");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::refresh + id + "refresh", UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesService->GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Refresh");

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::add + id, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            ImGui::OpenPopup((id + "CreateFilePopup").c_str());
        }
        UIUtil::RenderTooltip("Create new file...");

        if (ImGui::BeginPopup((id + "CreateFilePopup").c_str())) {
            if (ImGui::MenuItem((Icons::insert_drive_file + " Lua Script").c_str())) {
                filesService->CreateFile(filesContext.currentDirectory, "NewScript", Metal::FileExtensions::lua->extension);
                filesService->GetEntries(filesContext.currentDirectory);
            }
            if (ImGui::MenuItem((Icons::forest + " Level").c_str())) {
                filesService->CreateFile(filesContext.currentDirectory, "NewLevel", Metal::FileExtensions::level->extension);
                filesService->GetEntries(filesContext.currentDirectory);
            }
            if (ImGui::MenuItem((Icons::palette + " Material").c_str())) {
                filesService->CreateFile(filesContext.currentDirectory, "NewMaterial", Metal::FileExtensions::material->extension);
                filesService->GetEntries(filesContext.currentDirectory);
            }
            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 220.0f);
        if (ImGui::InputText((id + "AddressBar").c_str(), addressBuffer, sizeof(addressBuffer),
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::shared_ptr<FSEntry> newDir = filesService->GetEntry(addressBuffer);
            if (newDir && newDir->isDirectory) {
                filesContext.currentDirectory = newDir;
                filesService->GetEntries(newDir);
            } else {
                std::strncpy(addressBuffer, filesContext.currentDirectory->absolutePath.c_str(), sizeof(addressBuffer));
            }
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputTextWithHint("##Search", (Icons::search + " Search...").c_str(), filesContext.searchQuery,
                                 sizeof(filesContext.searchQuery));
    }
} // Metal
