#include "FilesListPanel.h"

#include "FilesPanel.h"
#include "FilesContext.h"
#include "../../util/UIUtil.h"
#include "../../../ApplicationContext.h"
#include "../../dto/FSEntry.h"
#include "../../service/FilesService.h"
#include <algorithm>

namespace Metal {
    void FilesListPanel::ensureCache() {
        if (!filesContext.currentDirectory) return;
        auto &children = filesContext.currentDirectory->children;
        if (lastDir != filesContext.currentDirectory || lastChildrenCount != children.size()) {
            cachedSorted.clear();
            for (auto *child: children) {
                cachedSorted.push_back(child);
            }
            lastDir = filesContext.currentDirectory;
            lastChildrenCount = children.size();
            applySort();
        }
    }

    void FilesListPanel::applySort() {
        if (!lastDir) return;
        std::sort(cachedSorted.begin(), cachedSorted.end(), [&](FSEntry *a, FSEntry *b) {
            const bool aIsDir = a->type == EntryType::DIRECTORY;
            const bool bIsDir = b->type == EntryType::DIRECTORY;
            // Always put directories first
            if (aIsDir != bIsDir) return aIsDir;
            int cmp = 0;
            switch (sortColumn) {
                case 0: // Name
                    cmp = a->name.compare(b->name);
                    break;
                case 1: // Date (formatted YYYY-MM-DD HH:MM)
                    cmp = a->formattedDate.compare(b->formattedDate);
                    break;
                case 2: // Type
                    cmp = static_cast<int>(a->type) - static_cast<int>(b->type);
                    break;
                case 3: // Size
                default:
                    if (a->size < b->size) cmp = -1;
                    else if (a->size > b->size) cmp = 1;
                    else cmp = 0;
                    break;
            }
            if (!sortAscending) cmp = -cmp;
            return cmp < 0;
        });
    }

    void FilesListPanel::updateDragStart() {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && startDrag.x == -1) {
            startDrag = ImGui::GetMousePos();
        }
    }

    void FilesListPanel::clearDragOnMouseUp() {
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            startDrag = ImVec2(-1, -1);
            onDrag = nullptr;
        }
    }

    void FilesListPanel::handleDrag() const {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && onDrag != nullptr) {
            UIUtil::AUX_VEC2 = ImGui::GetMousePos();
            UIUtil::AUX_VEC2.x += 10;
            UIUtil::AUX_VEC2.y += 10;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);
            ImGui::Begin((onDrag->getId() + std::string("drag")).c_str(), &UIUtil::OPEN,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::Text("Dragging Node %s", onDrag->name.c_str());
            ImGui::End();
        }
    }

    void FilesListPanel::handleDragDrop(FSEntry *fileEntry) {
        if (fileEntry != nullptr && fileEntry->isHovered && onDrag != fileEntry && startDrag.x >= 0) {
            if (onDrag == nullptr && ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                abs(startDrag.x - ImGui::GetMousePos().x) >= 3 &&
                abs(startDrag.y - ImGui::GetMousePos().y) >= 3) {
                onDrag = fileEntry;
            }

            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && onDrag != nullptr && fileEntry->type ==
                EntryType::DIRECTORY) {
                applicationContext->filesService.Move(onDrag, fileEntry);
                onDrag = nullptr;
            }
        }
    }

    void FilesListPanel::onClick(FSEntry *root) {
        if (root->isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                filesContext.selected.clear();
            }
            if (filesContext.selected.contains(root->getId()) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                filesContext.selected.erase(root->getId());
            } else {
                filesContext.selected[root->getId()] = root;
            }
        }
        if (root->isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            onDoubleClick(root);
        }
    }

    void FilesListPanel::renderTreeItem(FSEntry *entry) {
        if (entry == nullptr) {
            return;
        }

        const bool isDirectory = entry->type == EntryType::DIRECTORY;
        const bool passesFilter = filesContext.filterType == EntryType::NONE || entry->type == filesContext.filterType;
        if (!isDirectory && !passesFilter) {
            return;
        }

        const float rowHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().CellPadding.y * 2.0f;

        ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
        ImGui::TableNextColumn();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap;
        if (!isDirectory) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        const bool isSelected = filesContext.selected.contains(entry->getId());

        const bool isCut = filesContext.toCut.contains(entry->getId());
        if (isCut) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        }

        const std::string label = entry->name + id + entry->getId();

        const ImVec2 cursorPos = ImGui::GetCursorPos();
        ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns |
                                               ImGuiSelectableFlags_AllowItemOverlap |
                                               ImGuiSelectableFlags_AllowDoubleClick;
        ImGui::Selectable((id + std::string("row_") + entry->getId()).c_str(), isSelected, selectableFlags,
                          ImVec2(0.0f, rowHeight));

        entry->isHovered = ImGui::IsItemHovered();
        isSomethingHovered = isSomethingHovered || entry->isHovered;
        onClick(entry);
        handleDragDrop(entry);

        ImGui::SetCursorPos(cursorPos);

        bool open = false;
        const std::string icon = UIUtil::GetEntryIcon(entry->type);
        ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(0, 0, 0, 0));

        if (isDirectory) {
            open = ImGui::TreeNodeEx((icon + label).c_str(), flags);
        } else {
            ImGui::TreeNodeEx((icon + label).c_str(), flags);
        }

        ImGui::PopStyleColor(3);

        if (isCut) {
            ImGui::PopStyleColor();
        }

        UIUtil::RenderTooltip(entry->name);

        ImGui::TableNextColumn();
        if (!isDirectory) {
            ImGui::TextUnformatted(entry->formattedDate.c_str());
            ImGui::SetItemAllowOverlap();
        }

        ImGui::TableNextColumn();
        if (isDirectory) {
            ImGui::TextUnformatted("Directory");
            ImGui::SetItemAllowOverlap();
        } else {
            const char *typeLabel = "";
            switch (entry->type) {
                case EntryType::SCENE: typeLabel = "Scene";
                    break;
                case EntryType::MESH: typeLabel = "Mesh";
                    break;
                case EntryType::TEXTURE: typeLabel = "Texture";
                    break;
                case EntryType::VOLUME: typeLabel = "Volume";
                    break;
                default: typeLabel = "";
                    break;
            }
            ImGui::TextUnformatted(typeLabel);
            ImGui::SetItemAllowOverlap();
        }

        ImGui::TableNextColumn();
        if (!isDirectory) {
            ImGui::TextUnformatted(entry->formattedSize.c_str());
            ImGui::SetItemAllowOverlap();
        }

        if (isDirectory && open) {
            if (!loadedDirectoryPaths.contains(entry->absolutePath)) {
                FilesService::GetEntries(entry);
                loadedDirectoryPaths.insert(entry->absolutePath);
            }
            // After loading, children may change; keep original iteration order to avoid infinite recursion here.
            for (auto *child: entry->children) {
                renderTreeItem(child);
            }
            ImGui::TreePop();
        }
    }

    void FilesListPanel::onSync() {
        bool renderTree = ImGui::BeginChild((id + std::string("files_list")).c_str(), ImVec2(0, 0));
        if (renderTree) {
            isSomethingHovered = ImGui::IsWindowHovered();
            if (ImGui::IsWindowFocused()) {
                filesContext.selected.clear();
            }
            updateDragStart();
            handleDrag();

            constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg |
                                                   ImGuiTableFlags_BordersInnerV |
                                                   ImGuiTableFlags_Resizable |
                                                   ImGuiTableFlags_SizingStretchProp |
                                                   ImGuiTableFlags_ScrollY |
                                                   ImGuiTableFlags_Sortable;

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,
                                ImVec2(ImGui::GetStyle().CellPadding.x, 2.0f));
            if (ImGui::BeginTable((id + std::string("entries")).c_str(), 4, tableFlags)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn("Date");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Size");
                ImGui::TableHeadersRow();

                if (ImGuiTableSortSpecs *sortSpecs = ImGui::TableGetSortSpecs()) {
                    if (sortSpecs->SpecsCount > 0 && sortSpecs->SpecsDirty) {
                        const ImGuiTableColumnSortSpecs &s = sortSpecs->Specs[0];
                        sortColumn = s.ColumnIndex;
                        sortAscending = (s.SortDirection != ImGuiSortDirection_Descending);
                        applySort();
                        sortSpecs->SpecsDirty = false;
                    }
                }

                ensureCache();
                for (auto *child: cachedSorted) {
                    if (typeFilter != EntryType::NONE && child->type != typeFilter) {
                        continue;
                    }
                    renderTreeItem(child);
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            if (ImGui::BeginPopupContextItem((id + std::string("contextMenu")).c_str())) {
                if (ImGui::MenuItem("Cut")) {
                    filesContext.toCut.clear();
                    filesContext.toCut = filesContext.selected;
                }
                if (ImGui::MenuItem("Paste")) {
                    filesContext.toCut.clear();
                    FilesService::GetEntries(filesContext.currentDirectory);
                }
                if (ImGui::MenuItem("Delete")) {
                    applicationContext->filesService.deleteFiles(filesContext.selected);
                    FilesService::GetEntries(filesContext.currentDirectory);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Select all")) {
                    for (auto &entry: filesContext.currentDirectory->children) {
                        if (filesContext.filterType == EntryType::NONE || entry->type == filesContext.filterType) {
                            filesContext.selected.insert({entry->getId(), entry});
                        }
                    }
                }
                ImGui::EndPopup();
            }
            if (isSomethingHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup((id + std::string("contextMenu")).c_str());
            }

            clearDragOnMouseUp();
        }
        ImGui::EndChild();
    }
}
