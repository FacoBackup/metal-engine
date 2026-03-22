#include "FilesPanel.h"
#include "FilesHeaderPanel.h"
#include "ListDirectoryPanel.h"
#include "CardDirectoryPanel.h"
#include "NavigationPanel.h"
#include "../../../common/Icons.h"
#include "../../util/UIUtil.h"
#include "../../dto/FSEntry.h"
#include "../../dto/SceneImportSettingsDTO.h"
#include "FilesContext.h"
#include "../../../common/FileDialogUtil.h"
#include "../../../common/FileExtensions.h"
#include <string>
#include <filesystem>
#include "../../../common/FilesUtil.h"
#include "../../service/FileImporterService.h"
#include "../../repository/EditorRepository.h"
#include "../../service/SceneImporterService.h"
#include "../../service/FilesService.h"
#include "../../service/NotificationService.h"
#include "../../../engine/repository/WorldRepository.h"
#include "../../../engine/service/VoxelService.h"

namespace Metal {
    void FilesPanel::onInitialize() {
        filesContext.filesService = filesService;
        filesContext.setCurrentDirectory(filesService->getRoot());
        filesHeader = initializePanel<FilesHeaderPanel>(true, filesContext);
        listDirectoryPanel = initializePanel<ListDirectoryPanel>(
            false,
            filesContext,
            [this](const std::shared_ptr<FSEntry> &entry) { openResource(entry); }
        );
        cardDirectoryPanel = initializePanel<CardDirectoryPanel>(
            false,
            filesContext,
            [this](const std::shared_ptr<FSEntry> &entry) { openResource(entry); }
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
            if (renderHeader()) {
                filesHeader->onSync();
            }
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

    void FilesPanel::openResource(std::shared_ptr<FSEntry> root) {
        if (root->isDirectory) {
            filesContext.setCurrentDirectory(root);
            filesService->GetEntries(root);
            filesContext.selected.clear();
            return;
        }

        const std::string ext = root->extension;
        if (ext == EXT_SCENE) {
            notificationService->pushMessage("Loading scene", NotificationSeverities::SUCCESS);
            worldRepository->loadScene(root->getId());
        } else if (ext == EXT_SVO) {
            notificationService->pushMessage("Loading volume", NotificationSeverities::SUCCESS);
            voxelService->create(root->getId());
        } else if (fileImporterService->isCompatible(root->absolutePath)) {
            notificationService->pushMessage("Importing " + UIUtil::GetExtensionLabel(ext) + "...",
                                             NotificationSeverities::WARNING);
            fileImporterService->importFile(std::filesystem::path(root->absolutePath).parent_path().string(),
                                            root->absolutePath, std::make_shared<ImportSettingsDTO>());
        } else {
            notificationService->pushMessage("Unsupported resource type: " + ext, NotificationSeverities::ERROR);
        }
    }

    void FilesPanel::pasteSelected() {
        for (auto &entry: filesContext.toCut) {
            //
        }
        filesContext.toCut.clear();
        filesService->GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::openSelected() {
        for (auto &id: filesContext.selected) {
            openResource(id.second);
        }
    }

    void FilesPanel::cutSelected() {
        filesContext.toCut.clear();
        filesContext.toCut = filesContext.selected;
        filesService->GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::selectAll() {
        for (const auto &entry: filesContext.currentDirectory->children) {
            filesContext.selected.insert({entry->getId(), entry});
        }
    }

    void FilesPanel::deleteSelected() const {
        filesService->deleteFiles(filesContext.selected);
        filesService->GetEntries(filesContext.currentDirectory);
    }
}
