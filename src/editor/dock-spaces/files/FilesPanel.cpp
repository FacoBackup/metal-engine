#include "FilesPanel.h"
#include "FilesHeaderPanel.h"
#include "FilesListPanel.h"
#include "../../../common/Icons.h"
#include "../../util/UIUtil.h"
#include "../../dto/FSEntry.h"
#include "../../dto/SceneImportSettingsDTO.h"
#include "FilesContext.h"
#include "../../../common/FileDialogUtil.h"
#include "FilePreviewPanel.h"
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
    std::string FilesPanel::getActionLabel() {
        return Icons::file_open + "Import file";
    }

    std::function<void()> FilesPanel::onAction() {
        return [this] {
            auto files = FileDialogUtil::PickFiles({
                {
                    "Files",
                    fileImporterService->collectCompatibleFiles().c_str(),
                }
            });
            if (!files.empty()) {
                editorRepository->pendingImports = files;
                editorRepository->importSettingsMap.clear();
                for (const auto& file : editorRepository->pendingImports) {
                    if (sceneImporterService->isCompatible(file)) {
                        editorRepository->importSettingsMap.emplace(file, std::make_shared<SceneImportSettingsDTO>());
                    } else {
                        editorRepository->importSettingsMap.emplace(file, std::make_shared<ImportSettingsDTO>());
                    }
                }
                editorRepository->selectedFileForSettings = editorRepository->pendingImports[0];
                editorRepository->targetImportDirectory = filesContext.currentDirectory;
            }
        };
    }

    void FilesPanel::onInitialize() {
        filesContext.setCurrentDirectory(filesService->getRoot());
        filesHeader = initializePanel<FilesHeaderPanel>(true, filesContext, getActionLabel(), onAction());
        filesListPanel = initializePanel<FilesListPanel>(
            true,
            filesContext, [
                this](FSEntry *entry) {
                openResource(entry);
            },
            getTypeFilter());
        previewPanel = initializePanel<FilePreviewPanel>(true, filesContext);

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
            }),
            ShortcutDTO("Import File", ImGuiMod_Ctrl | ImGuiKey_I, onAction())
        };
    }

    void FilesPanel::onSync() {
        if (renderHeader()) {
            filesHeader->onSync();
        }
        ImGui::Separator();

        if (renderPreview()) {
            if (ImGui::BeginTable((id + "split_table").c_str(), 2,
                                  ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp)) {
                ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthStretch, 0.0f);
                ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, previewWidth);
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::BeginChild((id + "files_list_in_table").c_str(), ImVec2(0, 0),
                                  ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
                {
                    filesListPanel->onSync();
                }
                ImGui::EndChild();
                ImGui::TableNextColumn();
                ImGui::BeginChild((id + "preview_in_table").c_str(), ImVec2(0, 0));
                {
                    previewPanel->onSync();
                }
                ImGui::EndChild();
                previewWidth = ImGui::GetColumnWidth(1);
                ImGui::EndTable();
            }
        } else {
            filesListPanel->onSync();
        }
    }

    void FilesPanel::SetIconPos(const char *text) {
        const auto windowWidth = ImGui::GetWindowSize().x;
        const auto windowHeight = ImGui::GetWindowSize().y;
        const auto textWidth = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::SetCursorPosY((windowHeight - LARGE_FONT_SIZE * 2) * 0.5f);
    }

    void FilesPanel::openResource(FSEntry *root) {
        switch (root->type) {
            case EntryType::SCENE: {
                notificationService->pushMessage("Loading scene", NotificationSeverities::SUCCESS);
                worldRepository->loadScene(root->getId());
                break;
            }
            case EntryType::VOLUME: {
                notificationService->pushMessage("Loading volume", NotificationSeverities::SUCCESS);
                voxelService->create(root->getId());
                break;
            }
            case EntryType::DIRECTORY: {
                filesContext.setCurrentDirectory(root);
                filesService->GetEntries(root);
                filesContext.selected.clear();
                break;
            }
            default:
                notificationService->pushMessage("Unsupported resource type", NotificationSeverities::ERROR);
                break;
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
        for (auto &entry: filesContext.currentDirectory->children) {
            if (filesContext.filterType == EntryType::NONE || entry->type == filesContext.filterType) {
                filesContext.selected.insert({entry->getId(), entry});
            }
        }
    }

    void FilesPanel::deleteSelected() const {
        filesService->deleteFiles(filesContext.selected);
        filesService->GetEntries(filesContext.currentDirectory);
    }
}
