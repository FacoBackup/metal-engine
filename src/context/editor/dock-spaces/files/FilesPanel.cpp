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
        return [this] {
            auto files = FileDialogUtil::PickFiles({
                {
                    "Files",
                    CTX.fileImporterService.collectCompatibleFiles().c_str(),
                }
            });
            if (!files.empty()) {
                CTX.editorRepository.pendingImports = files;
                CTX.editorRepository.importSettingsMap.clear();
                for (const auto& file : CTX.editorRepository.pendingImports) {
                    if (CTX.sceneImporterService.isCompatible(file)) {
                        CTX.editorRepository.importSettingsMap.emplace(file, std::make_shared<SceneImportSettingsDTO>());
                    } else {
                        CTX.editorRepository.importSettingsMap.emplace(file, std::make_shared<ImportSettingsDTO>());
                    }
                }
                CTX.editorRepository.selectedFileForSettings = CTX.editorRepository.pendingImports[0];
                CTX.editorRepository.targetImportDirectory = filesContext.currentDirectory;
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
            case EntryType::MESH: {
                CTX.meshService.createMeshEntity(root->name, root->getId(), nullptr);
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
}
