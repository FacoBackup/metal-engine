#include "FilesPanel.h"


#include "FilesHeader.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/mesh/SceneData.h"
#include "../../../../util/UIUtil.h"
#include "../../../../dto/file//FileEntry.h"
#include "FilesContext.h"
#include "../../../../util/FileDialogUtil.h"

#include "FilePreviewPanel.h"
#include <string>

namespace Metal {
    std::string FilesPanel::getActionLabel() {
        return Icons::file_open + "Import file";
    }

    std::function<void()> FilesPanel::onAction() {
        return [this]() {
            auto files = FileDialogUtil::PickFiles({
                {
                    "Files",
                    context->fileImporterService.collectCompatibleFiles().c_str(),
                }
            });
            for (const std::string &file: files) {
                context->fileImporterService.importFile(filesContext.currentDirectory->absolutePath, file);
            }
            FilesService::GetEntries(filesContext.currentDirectory);
        };
    }

    void FilesPanel::onInitialize() {
        filesContext.setCurrentDirectory(context->filesService.getRoot());
        appendChild(filesHeader = new FilesHeader(filesContext, getActionLabel(), onAction()));
        previewPanel = new FilePreviewPanel(filesContext);
        appendChild(previewPanel);
    }

    void FilesPanel::contextMenu() {
        if (ImGui::BeginPopupContextItem((id + "contextMenu").c_str())) {
            if (ImGui::MenuItem("Cut")) {
                cutSelected();
            }
            if (ImGui::MenuItem("Paste")) {
                pasteSelected();
            }
            if (ImGui::MenuItem("Delete")) {
                deleteSelected();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Select all")) {
                selectAll();
            }
            ImGui::EndPopup();
        }

        if (isSomethingHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup((id + "contextMenu").c_str());
        }
    }

    void FilesPanel::handleDrag() const {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && onDrag != nullptr) {
            UIUtil::AUX_VEC2 = ImGui::GetMousePos();
            UIUtil::AUX_VEC2.x += 10;
            UIUtil::AUX_VEC2.y += 10;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);
            ImGui::Begin((onDrag->getId() + "drag").c_str(), &UIUtil::OPEN,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::Text("Dragging Node %s", onDrag->name.c_str());
            ImGui::End();
        }
    }

    void FilesPanel::onSync() {
        filesHeader->onSync();
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
                                                           ImGuiTableFlags_ScrollY;

                    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,
                                        ImVec2(ImGui::GetStyle().CellPadding.x, 2.0f));
                    if (ImGui::BeginTable((id + "entries").c_str(), 4, tableFlags)) {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                        ImGui::TableSetupColumn("Date");
                        ImGui::TableSetupColumn("Type");
                        ImGui::TableSetupColumn("Size");
                        ImGui::TableHeadersRow();

                        for (auto *child: filesContext.currentDirectory->children) {
                            renderTreeItem(child);
                        }

                        ImGui::EndTable();
                    }
                    ImGui::PopStyleVar();

                    contextMenu();
                    clearDragOnMouseUp();
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
            bool renderTree = ImGui::BeginChild((id + "files_list").c_str(), ImVec2(0, 0));
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
                                                       ImGuiTableFlags_ScrollY;

                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,
                                    ImVec2(ImGui::GetStyle().CellPadding.x, 2.0f));
                if (ImGui::BeginTable((id + "entries").c_str(), 4, tableFlags)) {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                    ImGui::TableSetupColumn("Date");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupColumn("Size");
                    ImGui::TableHeadersRow();

                    for (auto *child: filesContext.currentDirectory->children) {
                        renderTreeItem(child);
                    }

                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();

                contextMenu();
                clearDragOnMouseUp();
            }
            ImGui::EndChild();
        }
    }

    void FilesPanel::SetIconPos(const char *text) {
        const auto windowWidth = ImGui::GetWindowSize().x;
        const auto windowHeight = ImGui::GetWindowSize().y;
        const auto textWidth = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::SetCursorPosY((windowHeight - LARGE_FONT_SIZE * 2) * 0.5f);
    }

    void FilesPanel::updateDragStart() {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && startDrag.x == -1) {
            startDrag = ImGui::GetMousePos();
        }
    }

    void FilesPanel::clearDragOnMouseUp() {
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            startDrag = ImVec2(-1, -1);
            onDrag = nullptr;
        }
    }

    void FilesPanel::renderTreeItem(FileEntry *entry) {
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
        ImGui::Selectable((id + "row_" + entry->getId()).c_str(), isSelected, selectableFlags, ImVec2(0.0f, rowHeight));

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
                case EntryType::MATERIAL: typeLabel = "Material";
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

            for (auto *child: entry->children) {
                renderTreeItem(child);
            }
            ImGui::TreePop();
        }
    }

    void FilesPanel::handleDragDrop(FileEntry *fileEntry) {
        if (fileEntry != nullptr && fileEntry->isHovered && onDrag != fileEntry && startDrag.x >= 0) {
            if (onDrag == nullptr && ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                abs(startDrag.x - ImGui::GetMousePos().x) >= 3 &&
                abs(startDrag.y - ImGui::GetMousePos().y) >= 3) {
                onDrag = fileEntry;
            }

            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && onDrag != nullptr && fileEntry->type ==
                EntryType::DIRECTORY) {
                context->filesService.Move(onDrag, fileEntry);
                onDrag = nullptr;
            }
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
        context->filesService.deleteFiles(filesContext.selected);
        FilesService::GetEntries(filesContext.currentDirectory);
    }

    void FilesPanel::onClick(FileEntry *root) {
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
            openResource(root);
        }
    }

    void FilesPanel::openResource(FileEntry *root) {
        switch (root->type) {
            case EntryType::MESH: {
                context->meshService.createMeshEntity(root->name, root->getId());
                break;
            }
            case EntryType::SCENE: {
                context->meshService.createSceneEntities(root->getId());
                break;
            }
            case EntryType::VOLUME: {
                context->voxelService.create(root->getId());
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
}
