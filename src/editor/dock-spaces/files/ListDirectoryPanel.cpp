#include "ListDirectoryPanel.h"
#include "../../dto/FSEntry.h"
#include "../../service/FilesService.h"
#include "../../../common/Icons.h"
#include "../../util/UIUtil.h"

namespace Metal {
    void ListDirectoryPanel::onClick(const std::shared_ptr<FSEntry> &entry) {
        if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
            if (filesContext.selected.contains(entry->getId())) {
                filesContext.selected.erase(entry->getId());
            } else {
                filesContext.selected.insert({entry->getId(), entry});
            }
        } else {
            filesContext.selected.clear();
            filesContext.selected.insert({entry->getId(), entry});
        }
    }

    void ListDirectoryPanel::renderTreeItem(const std::shared_ptr<FSEntry> &entry) {
        const bool isDirectory = entry->isDirectory;
        const bool isSelected = filesContext.selected.contains(entry->getId());
        const bool interactable = isInteractable(entry);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        if (!interactable) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_OpenOnArrow;
        if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
        if (!isDirectory) flags |= ImGuiTreeNodeFlags_Leaf;

        const std::string icon = isDirectory ? Icons::folder : UIUtil::GetExtensionIcon(entry->extension);
        const std::string label = icon + " " + entry->name;

        bool open = ImGui::TreeNodeEx((id + entry->getId()).c_str(), flags, "%s", label.c_str());

        if (ImGui::IsItemClicked()) {
            onClick(entry);
        }

        if (ImGui::IsItemHovered()) {
            renderInfoTooltip(entry);
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                onDoubleClick(entry);
            }
        }

        ImGui::TableNextColumn();
        if (!isDirectory) {
            ImGui::TextUnformatted(entry->formattedDate.c_str());
        }

        ImGui::TableNextColumn();
        if (isDirectory) {
            ImGui::TextUnformatted("Directory");
        } else {
            ImGui::TextUnformatted(UIUtil::GetExtensionLabel(entry->extension).c_str());
        }

        ImGui::TableNextColumn();
        if (!isDirectory) {
            ImGui::TextUnformatted(entry->formattedSize.c_str());
        }

        if (!interactable) {
            ImGui::PopStyleColor();
        }

        if (open) {
            if (isDirectory) {
                filesService->GetEntries(entry);
                for (const auto &child: entry->children) {
                    renderTreeItem(child);
                }
            }
            ImGui::TreePop();
        } else if (isDirectory) {
            entry->children.clear();
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

                for (const auto &child: filesContext.currentDirectory->children) {
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
