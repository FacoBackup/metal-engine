#include "FilesPanel.h"
#include "FilesHeaderPanel.h"
#include "ListDirectoryPanel.h"
#include "CardDirectoryPanel.h"
#include "NavigationPanel.h"
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "editor/dto/FSEntry.h"
#include "../../dto/SceneImportSettingsDTO.h"
#include "FilesContext.h"
#include "common/FileDialogUtil.h"
#include "common/FileExtensions.h"
#include <string>
#include <filesystem>
#include "common/FilesUtil.h"
#include "../../service/FileImporterService.h"
#include "../../repository/EditorRepository.h"
#include "../../service/SceneImporterService.h"
#include "../../service/FilesService.h"
#include "../../service/NotificationService.h"
#include "engine/repository/WorldRepository.h"
#include "engine/service/VoxelService.h"

namespace Metal {
    void FilesPanel::onInitialize() {
        filesContext.filesService = filesService;
        filesContext.currentDirectory = filesService->getRoot();
        filesHeader = initializePanel<FilesHeaderPanel>(true, filesContext);
        listDirectoryPanel = initializePanel<ListDirectoryPanel>(
            false,
            filesContext
        );
        cardDirectoryPanel = initializePanel<CardDirectoryPanel>(
            false,
            filesContext
        );
        navigationPanel = initializePanel<NavigationPanel>(false, filesContext);

        shortcuts = {
            ShortcutDTO("Cut", ImGuiMod_Ctrl | ImGuiKey_X, [this]() {
                cutSelected();
            }),
            ShortcutDTO("Paste", ImGuiMod_Ctrl | ImGuiKey_V, [this]() {
                pasteSelected();
            }),
            ShortcutDTO("Delete", ImGuiKey_Delete, [this]() {
                deleteSelected();
            }),
            ShortcutDTO("Select All", ImGuiMod_Ctrl | ImGuiKey_A, [this]() {
                selectAll();
            })
        };
    }

    void FilesPanel::onSync() {
        if (ImGui::BeginTable("FilesPanelLayout", 2, ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Navigation", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            navigationPanel->onSync();

            ImGui::TableNextColumn();
            filesHeader->onSync();
            ImGui::Separator();

            if (filesContext.viewMode == FilesViewMode::LIST) {
                listDirectoryPanel->onSync();
            } else {
                cardDirectoryPanel->onSync();
            }
            ImGui::EndTable();
        }
    }

    void FilesPanel::SetIconPos(const char *text) {
        const auto windowWidth = ImGui::GetWindowSize().x;
        const auto windowHeight = ImGui::GetWindowSize().y;
        const auto textWidth = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::SetCursorPosY((windowHeight - LARGE_FONT_SIZE * 2) * 0.5f);
    }


    void FilesPanel::pasteSelected() {
        for (auto &entry: filesContext.toCut) {
            //
        }
        filesContext.toCut.clear();
        filesService->GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::openSelected() {
        std::vector<std::shared_ptr<FSEntry> > toOpen;
        for (auto &id: filesContext.selected) {
            toOpen.push_back(id.second);
        }

        for (auto &entry: toOpen) {
            if (filesContext.viewMode == FilesViewMode::LIST) {
                listDirectoryPanel->openResource(entry);
            } else {
                cardDirectoryPanel->openResource(entry);
            }
        }
    }

    void FilesPanel::cutSelected() {
        filesContext.toCut.clear();
        filesContext.toCut = filesContext.selected;
        filesService->GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::selectAll() {
        auto currentDir = filesContext.currentDirectory;
        for (const auto &entry: currentDir->children) {
            filesContext.selected.insert({entry->getAbsolutePath(), entry});
        }
    }

    void FilesPanel::deleteSelected() const {
        auto selectedCopy = filesContext.selected;
        filesService->deleteFiles(selectedCopy);
        filesService->GetEntries(filesContext.currentDirectory);
    }
}
