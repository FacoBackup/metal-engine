#include "FileImportModalPanel.h"

#include <filesystem>
#include <imgui.h>
#include "../../ApplicationContext.h"
#include "../../common/FilesUtil.h"
#include "../abstract/form/FormPanel.h"

namespace Metal {
    void FileImportModalPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
    }

    void FileImportModalPanel::onSync() {
        auto &editorRepository = applicationContext->editorRepository;
        if (editorRepository.pendingImports.empty()) {
            isFirst = true;
            return;
        }

        if (isFirst) {
            formPanel->setInspection(
                editorRepository.importSettingsMap.at(editorRepository.selectedFileForSettings).get());
            isFirst = false;
        }

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        const float maxHeight = viewport->Size.y * 0.7f;
        const float width = 800.0f;

        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSizeConstraints(ImVec2(width, 400), ImVec2(width, maxHeight));

        if (ImGui::Begin(("Import files?" + id + "importModal").c_str(), nullptr,
                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove)) {
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

                    for (const auto &filePath: editorRepository.pendingImports) {
                        std::filesystem::path p(filePath);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool isSelected = (editorRepository.selectedFileForSettings == filePath);
                        if (ImGui::Selectable(p.filename().string().c_str(), isSelected,
                                              ImGuiSelectableFlags_SpanAllColumns)) {
                            editorRepository.selectedFileForSettings = filePath;
                            formPanel->setInspection(
                                editorRepository.importSettingsMap.at(editorRepository.selectedFileForSettings).
                                get());
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
                ImGui::Text("Settings: %s",
                            std::filesystem::path(editorRepository.selectedFileForSettings).filename().string().
                            c_str());
                ImGui::BeginChild((id + "SettingsFormChild").c_str(), ImVec2(0, maxHeight - 180.0f), true);
                formPanel->onSync();
                ImGui::EndChild();

                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button(("Approve" + id + "approveImport").c_str(), ImVec2(120, 0))) {
                for (const std::string &file: editorRepository.pendingImports) {
                    applicationContext->fileImporterService.importFile(editorRepository.targetImportDirectory->absolutePath, file,
                                                       editorRepository.importSettingsMap.at(file));
                }
                applicationContext->notificationService.pushMessage("Importing files...", NotificationSeverities::WARNING);
                FilesService::GetEntries(editorRepository.targetImportDirectory);
                editorRepository.pendingImports.clear();
                editorRepository.importSettingsMap.clear();
                formPanel->resetForm();
            }
            ImGui::SameLine();
            if (ImGui::Button(("Cancel" + id + "cancel").c_str(), ImVec2(120, 0))) {
                editorRepository.pendingImports.clear();
                editorRepository.importSettingsMap.clear();
                formPanel->resetForm();
            }
        }
        ImGui::End();
    }
} // Metal
