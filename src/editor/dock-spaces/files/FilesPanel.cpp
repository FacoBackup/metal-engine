#include "FilesPanel.h"


#include "FilesHeaderPanel.h"
#include "FilesListPanel.h"
#include "../../../common/Icons.h"
#include "../../../ApplicationContext.h"
#include "../../dto/SceneData.h"
#include "../../util/UIUtil.h"
#include "../../dto/FSEntry.h"
#include "../../dto/SceneImportSettingsDTO.h"
#include "FilesContext.h"
#include "../../../common/FileDialogUtil.h"

#include "FilePreviewPanel.h"
#include "../../abstract/form/FormPanel.h"
#include <string>
#include <filesystem>
#include "../../../common/FilesUtil.h"

namespace Metal {
    std::string FilesPanel::getActionLabel() {
        return Icons::file_open + "Import file";
    }

    std::function<void()> FilesPanel::onAction() {
        return [this] {
            auto files = FileDialogUtil::PickFiles({
                {
                    "Files",
                    applicationContext->fileImporterService.collectCompatibleFiles().c_str(),
                }
            });
            if (!files.empty()) {
                applicationContext->editorRepository.pendingImports = files;
                applicationContext->editorRepository.importSettingsMap.clear();
                for (const auto& file : applicationContext->editorRepository.pendingImports) {
                    if (applicationContext->sceneImporterService.isCompatible(file)) {
                        applicationContext->editorRepository.importSettingsMap.emplace(file, std::make_shared<SceneImportSettingsDTO>());
                    } else {
                        applicationContext->editorRepository.importSettingsMap.emplace(file, std::make_shared<ImportSettingsDTO>());
                    }
                }
                applicationContext->editorRepository.selectedFileForSettings = applicationContext->editorRepository.pendingImports[0];
                applicationContext->editorRepository.targetImportDirectory = filesContext.currentDirectory;
            }
        };
    }

    void FilesPanel::onInitialize() {
        filesContext.setCurrentDirectory(applicationContext->filesService.getRoot());
        appendChild(filesHeader = new FilesHeaderPanel(filesContext, getActionLabel(), onAction()));
        filesListPanel = new FilesListPanel(
            filesContext, [
                this](FSEntry *entry) {
                openResource(entry);
            },
            getTypeFilter());
        appendChild(filesListPanel);
        previewPanel = new FilePreviewPanel(filesContext);
        appendChild(previewPanel);

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
                applicationContext->notificationService.pushMessage("Loading scene", NotificationSeverities::SUCCESS);
                applicationContext->worldRepository.loadScene(root->getId());
                break;
            }
            case EntryType::VOLUME: {
                applicationContext->notificationService.pushMessage("Loading volume", NotificationSeverities::SUCCESS);
                applicationContext->voxelService.create(root->getId());
                break;
            }
            case EntryType::DIRECTORY: {
                filesContext.setCurrentDirectory(root);
                FilesService::GetEntries(root);
                filesContext.selected.clear();
                break;
            }
            default:
                applicationContext->notificationService.pushMessage("Unsupported resource type", NotificationSeverities::ERROR);
                break;
        }
    }

    void FilesPanel::pasteSelected() {
        for (auto &entry: filesContext.toCut) {
            //
        }
        filesContext.toCut.clear();
        FilesService::GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::openSelected() {
        for (auto &id: filesContext.selected) {
            openResource(id.second);
        }
    }

    void FilesPanel::cutSelected() {
        filesContext.toCut.clear();
        filesContext.toCut = filesContext.selected;
        FilesService::GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::selectAll() {
        for (auto &entry: filesContext.currentDirectory->children) {
            if (filesContext.filterType == EntryType::NONE || entry->type == filesContext.filterType) {
                filesContext.selected.insert({entry->getId(), entry});
            }
        }
    }

    void FilesPanel::deleteSelected() const {
        applicationContext->filesService.deleteFiles(filesContext.selected);
        FilesService::GetEntries(filesContext.currentDirectory);
    }
}
