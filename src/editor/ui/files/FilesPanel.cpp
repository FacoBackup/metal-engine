#include "FilesPanel.h"

#include "FilesHeader.h"
#include "../../../common/interface/Icons.h"
#include "../../../common/util/files/FilesUtil.h"
#include "../../../context/ApplicationContext.h"
#include "../../common/UIUtil.h"
#include "../../../common/util/files/FileEntry.h"
#include "FilesContext.h"

namespace Metal {
    void FilesPanel::onInitialize() {
        filesContext.setCurrentDirectory(context->getEditorContext().filesService.getRoot());
        appendChild(new FilesHeader(filesContext));
    }

    void FilesPanel::handleDrag() const {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && onDrag != nullptr) {
            UIUtil::AUX_VEC2 = ImGui::GetMousePos();
            UIUtil::AUX_VEC2.x += 10;
            UIUtil::AUX_VEC2.y += 10;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);
            ImGui::Begin((onDrag->id + "drag").c_str(), &UIUtil::OPEN,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::Text("Dragging Node %s", onDrag->name.c_str());
            ImGui::End();
        }
    }

    void FilesPanel::onSync() {
        onSyncChildren();
        if (ImGui::BeginChild(id.c_str())) {
            isSomethingHovered = ImGui::IsWindowHovered();
            if (ImGui::IsWindowFocused()) {
                filesContext.selected.clear();
            }

            handleDrag();

            float size = std::round(ImGui::GetWindowSize().x / CARD_SIZE) * CARD_SIZE - CARD_SIZE;
            int rowIndex = 1;
            for (auto &child: filesContext.currentDirectory->children) {
                const bool isSelected = filesContext.selected.contains(child->id) || child->isHovered && onDrag !=
                                        nullptr;
                ImGui::PushStyleColor(ImGuiCol_ChildBg, isSelected
                                                            ? context->getEditorContext().editorRepository.accent
                                                            : context->getEditorContext().themeService.palette0);
                renderItem(child);
                ImGui::PopStyleColor();

                if (rowIndex * CARD_SIZE < size) {
                    ImGui::SameLine();
                    rowIndex++;
                } else {
                    rowIndex = 0;
                }
                hotkeys();
            }
            trackDrag();
        }

        ImGui::EndChild();
    }

    void FilesPanel::renderItem(FileEntry *root) {
        UIUtil::AUX_VEC2.x = CARD_SIZE;
        UIUtil::AUX_VEC2.y = CARD_SIZE + 15;
        if (ImGui::BeginChild(root->id.c_str(), UIUtil::AUX_VEC2, ImGuiChildFlags_Border)) {
            root->isHovered = ImGui::IsWindowHovered();
            isSomethingHovered = isSomethingHovered || root->isHovered;
            onClick(root);
            handleDragDrop(root);
            if (root->type == EntryType::DIRECTORY) {
                ImGui::TextColored(UIUtil::DIRECTORY_COLOR, Icons::folder.c_str());
            } // else if (fEntry->type ==  EntryType::TEXTURE) {
            // var texture = streamingService.streamIn(child, StreamableResourceType.TEXTURE);
            // if (texture != null) {
            //     texture.lastUse = clockRepository.totalTime;
            //     ImGui::image(((TextureResourceRef) texture).texture, TEXTURE_SIZE, PreviewField.INV_X_L, INV_Y);
            // }
            // }
            else {
                ImGui::Text(UIUtil::GetFileIcon(root->type).c_str());
            }
            UIUtil::AUX_VEC2.x = 0;
            UIUtil::AUX_VEC2.y = ImGui::GetContentRegionAvail().y - TEXT_OFFSET;
            ImGui::Dummy(UIUtil::AUX_VEC2);
            ImGui::Separator();
            if (filesContext.toCut.contains(root->id)) {
                ImGui::TextDisabled(root->name.c_str());
            } else {
                ImGui::Text(root->name.c_str());
            }
        }
        ImGui::EndChild();
    }

    void FilesPanel::trackDrag() {
        if (startDrag.x == -1 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            startDrag.x = ImGui::GetMousePos().x;
            startDrag.y = ImGui::GetMousePos().y;
        }

        if (startDrag.x != -1 && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            startDrag.x = startDrag.y = -1;
            onDrag = nullptr;
        }
    }

    void FilesPanel::handleDragDrop(FileEntry *fileEntry) {
        if (fileEntry->isHovered && onDrag != fileEntry && startDrag.x >= 0) {
            if (onDrag == nullptr && ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                abs(startDrag.x - ImGui::GetMousePos().x) >= 3 &&
                abs(startDrag.y - ImGui::GetMousePos().y) >= 3) {
                onDrag = fileEntry;
            }

            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && onDrag != nullptr && fileEntry->type ==
                EntryType::DIRECTORY) {
                FilesService::Move(onDrag, fileEntry);
                onDrag = nullptr;
            }
        }
    }

    void FilesPanel::hotkeys() {
        if (!isSomethingHovered) {
            return;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) && !filesContext.selected.empty()) {
            openSelected();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !filesContext.selected.empty()) {
            deleteSelected();
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_A)) {
            selectAll();
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_X)) {
            cutSelected();
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V)) {
            pasteSelected();
        }
    }

    void FilesPanel::pasteSelected() {
        for (auto &entry: filesContext.toCut) {
            // var children = filesRepository.parentChildren.get(currentDirectory);
            // if (!children.contains(key)) {
            //     var previousParentId = filesRepository.childParent.get(key);
            //     filesRepository.parentChildren.get(previousParentId).remove(key);
            //
            //     children.add(key);
            //     filesRepository.childParent.put(key, currentDirectory);
            // }
        }
        filesContext.toCut.clear();
    }

    void FilesPanel::openSelected() {
        for (auto &id: filesContext.selected) {
            openResource(id.second);
        }
    }

    void FilesPanel::cutSelected() {
        filesContext.toCut.clear();
        filesContext.toCut = filesContext.selected;
    }

    void FilesPanel::selectAll() {
        for (auto &entry: filesContext.currentDirectory->children) {
            filesContext.selected[entry->id] = entry;
        }
    }

    void FilesPanel::deleteSelected() {
        // filesService.deleteSelected(selected.keySet());
    }

    void FilesPanel::onClick(FileEntry *root) {
        if (root->isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                filesContext.selected.clear();
            }
            if (filesContext.selected.contains(root->id) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                filesContext.selected.erase(root->id);
            } else {
                filesContext.selected[root->id] = root;
            }
        }
        if (root->isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            openResource(root);
        }
    }

    void FilesPanel::openResource(FileEntry *root) {
        switch (root->type) {
            case EntryType::MESH: {
                context->getVulkanContext().corePipelines.sampleMesh = context->getVulkanContext().meshService.
                        createMesh(root->associatedFiles[0]);
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
} // Metal
