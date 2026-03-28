#include "ListDirectoryPanel.h"
#include "editor/dto/FSEntry.h"
#include "../../service/FilesService.h"
#include "editor/ui/UIUtil.h"
#include "common/AbstractImporter.h"

namespace Metal {
    void ListDirectoryPanel::onClick(std::shared_ptr<FSEntry> entry) {
        if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
            if (filesContext.selected.contains(entry->getAbsolutePath())) {
                filesContext.selected.erase(entry->getAbsolutePath());
            } else {
                filesContext.selected.insert({entry->getAbsolutePath(), entry});
            }
        } else {
            filesContext.selected.clear();
            filesContext.selected.insert({entry->getAbsolutePath(), entry});
        }
    }

    void ListDirectoryPanel::renderTreeItem(std::shared_ptr<FSEntry> entry) {
        const bool isDirectory = entry->isDirectory;
        const bool isSelected = filesContext.selected.contains(entry->getAbsolutePath());

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        const std::string icon = getEntryIcon(entry);
        const std::string label = icon + " " + entry->name;

        if (ImGui::Selectable(label.c_str(), isSelected,
                              ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
            onClick(entry);
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                openResource(entry);
            }
        }

        if (ImGui::IsItemHovered()) {
            renderInfoTooltip(entry);
        }

        ImGui::TableNextColumn();
        if (!isDirectory) {
            ImGui::TextUnformatted(entry->formattedDate.c_str());
        }

        ImGui::TableNextColumn();
        if (isDirectory) {
            ImGui::TextUnformatted("Directory");
        } else {
            ImGui::TextUnformatted(getExtensionLabel(entry->extension).c_str());
        }

        ImGui::TableNextColumn();
        if (!isDirectory) {
            ImGui::TextUnformatted(entry->formattedSize.c_str());
        }
    }

    void ListDirectoryPanel::onSync() {
        if (ImGui::BeginChild((id + "list_view").c_str(), ImVec2(0, 0))) {
            if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemHovered()) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    filesContext.selected.clear();
                }
            }

            constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg |
                                                   ImGuiTableFlags_BordersInnerV |
                                                   ImGuiTableFlags_Resizable |
                                                   ImGuiTableFlags_SizingStretchProp |
                                                   ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable((id + "entries").c_str(), 4, tableFlags)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                auto currentDir = filesContext.currentDirectory;
                for (const auto &child: currentDir->children) {
                    if (matchesSearch(child)) {
                        renderTreeItem(child);
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }
}
