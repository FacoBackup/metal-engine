#include "FilesPanel.h"


#include "FilesHeaderPanel.h"
#include "FilesListPanel.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/mesh/SceneData.h"
#include "../../../../util/UIUtil.h"
#include "../../../../dto/file/FSEntry.h"
#include "../../../../dto/file/SceneImportSettingsDTO.h"
#include "FilesContext.h"
#include "../../../../util/FileDialogUtil.h"

#include "FilePreviewPanel.h"
#include "../../abstract/form/FormPanel.h"
#include <string>
#include <filesystem>
#include "../../../../util/FilesUtil.h"

namespace Metal {
    std::string FilesPanel::getActionLabel() {
        return Icons::file_open + "Import file";
    }

    std::function<void()> FilesPanel::onAction() {
        return [this]() {
            auto files = FileDialogUtil::PickFiles({
                {
                    "Files",
                    CTX.fileImporterService.collectCompatibleFiles().c_str(),
                }
            });
            if (!files.empty()) {
                pendingImports = files;
                importSettingsMap.clear();
                for (const auto& file : pendingImports) {
                    if (CTX.sceneImporterService.isCompatible(file)) {
                        importSettingsMap.emplace(file, std::make_shared<SceneImportSettingsDTO>());
                    } else {
                        importSettingsMap.emplace(file, std::make_shared<ImportSettingsDTO>());
                    }
                }
                selectedFileForSettings = pendingImports[0];
                if (settingsForm) {
                    settingsForm->setInspection(importSettingsMap.at(selectedFileForSettings).get());
                }
                ImGui::OpenPopup(("Import files?" + id + "importModal").c_str());
            }
        };
    }

    void FilesPanel::onInitialize() {
        filesContext.setCurrentDirectory(CTX.filesService.getRoot());
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

        settingsForm = new FormPanel();
        appendChild(settingsForm);

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

        renderImportModal();

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
            case EntryType::MESH: {
                CTX.meshService.createMeshEntity(root->name, root->getId(), "");
                break;
            }
            case EntryType::SCENE: {
                CTX.meshService.createSceneEntities(root->getId());
                break;
            }
            case EntryType::VOLUME: {
                CTX.voxelService.create(root->getId());
                break;
            }
            case EntryType::DIRECTORY: {
                filesContext.setCurrentDirectory(root);
                FilesService::GetEntries(root);
                filesContext.selected.clear();
                break;
            }
            default:
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
        CTX.filesService.deleteFiles(filesContext.selected);
        FilesService::GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::renderImportModal() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        const float maxHeight = viewport->Size.y * 0.7f;
        const float width = 800.0f;

        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSizeConstraints(ImVec2(width, 400), ImVec2(width, maxHeight));

        if (ImGui::BeginPopupModal(("Import files?" + id + "importModal").c_str(), nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {

            if (ImGui::BeginTable((id + "ImportLayoutTable").c_str(), 2, ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("FileList", ImGuiTableColumnFlags_WidthStretch, 0.4f);
                ImGui::TableSetupColumn("Settings", ImGuiTableColumnFlags_WidthStretch, 0.6f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::Text("Files:");
                if (ImGui::BeginTable((id + "ImportFilesTable").c_str(), 3,
                                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
                                      ImVec2(0, maxHeight - 180.0f))) {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Ext", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    ImGui::TableHeadersRow();

                    for (const auto &filePath: pendingImports) {
                        std::filesystem::path p(filePath);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool isSelected = (selectedFileForSettings == filePath);
                        if (ImGui::Selectable(p.filename().string().c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                            selectedFileForSettings = filePath;
                            settingsForm->setInspection(importSettingsMap.at(selectedFileForSettings).get());
                        }
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(p.extension().string().c_str());
                        ImGui::TableNextColumn();
                        try {
                            ImGui::TextUnformatted(FilesUtil::FormatSize(std::filesystem::file_size(p)).c_str());
                        } catch (...) {
                            ImGui::TextUnformatted("N/A");
                        }
                    }
                    ImGui::EndTable();
                }

                ImGui::TableNextColumn();
                ImGui::Text("Settings: %s", std::filesystem::path(selectedFileForSettings).filename().string().c_str());
                ImGui::BeginChild((id + "SettingsFormChild").c_str(), ImVec2(0, maxHeight - 180.0f), true);
                settingsForm->onSync();
                ImGui::EndChild();

                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button(("Approve" + id + "approveImport").c_str(), ImVec2(120, 0))) {
                for (const std::string &file: pendingImports) {
                    CTX.fileImporterService.importFile(filesContext.currentDirectory->absolutePath, file, importSettingsMap.at(file));
                }
                CTX.notificationService.pushMessage("Importing files...", NotificationSeverities::WARNING);
                FilesService::GetEntries(filesContext.currentDirectory);
                pendingImports.clear();
                importSettingsMap.clear();
                settingsForm->resetForm();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(("Cancel" + id + "cancel").c_str(), ImVec2(120, 0))) {
                pendingImports.clear();
                importSettingsMap.clear();
                settingsForm->resetForm();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}
