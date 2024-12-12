#include "FilesPanel.h"

#include "FilesHeader.h"
#include "../../../common/interface/Icons.h"
#include "../../../common/util/files/FilesUtil.h"
#include "../../../context/ApplicationContext.h"
#include "../../common/UIUtil.h"

namespace Metal {
    void FilesPanel::onInitialize() {
        filesContext = std::make_unique<FilesContext>(context->getRootDirectory());
        FilesUtil::GetEntries(*filesContext->currentDirectory);
        appendChild(new FilesHeader(filesContext));
    }

    void FilesPanel::onSync() {
        onSyncChildren();
        if (ImGui::BeginChild(id.c_str())) {
            isSomethingHovered = ImGui::IsWindowHovered();
            if (ImGui::IsWindowFocused()) {
                filesContext->selected.clear();
            }
            float size = std::round(ImGui::GetWindowSize().x / CARD_SIZE) * CARD_SIZE - CARD_SIZE;
            int rowIndex = 1;
            for (auto &child: filesContext->currentDirectory->children) {
                const bool isSelected = filesContext->selected.contains(child.id);
                ImGui::PushStyleColor(ImGuiCol_ChildBg,
                                      isSelected || child.isHovered
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
        }
        ImGui::EndChild();
    }

    void FilesPanel::renderItem(FileEntry &fEntry) {
        UIUtil::AUX_VEC2.x = CARD_SIZE;
        UIUtil::AUX_VEC2.y = CARD_SIZE + 15;
        if (ImGui::BeginChild(fEntry.id.c_str(), UIUtil::AUX_VEC2, ImGuiChildFlags_Border)) {
            fEntry.isHovered = ImGui::IsWindowHovered();
            isSomethingHovered = isSomethingHovered || fEntry.isHovered;
            onClick(fEntry);
            if (fEntry.type == EntryType::DIRECTORY) {
                ImGui::TextColored(UIUtil::DIRECTORY_COLOR, Icons::folder.c_str());
            } // else if (fEntry.type ==  EntryType::TEXTURE) {
            // var texture = streamingService.streamIn(child, StreamableResourceType.TEXTURE);
            // if (texture != null) {
            //     texture.lastUse = clockRepository.totalTime;
            //     ImGui::image(((TextureResourceRef) texture).texture, TEXTURE_SIZE, PreviewField.INV_X_L, INV_Y);
            // }
            // }
            else {
                ImGui::Text(UIUtil::GetFileIcon(fEntry.type).c_str());
            }
            UIUtil::AUX_VEC2.x = 0;
            UIUtil::AUX_VEC2.y = ImGui::GetContentRegionAvail().y - TEXT_OFFSET;
            ImGui::Dummy(UIUtil::AUX_VEC2);
            ImGui::Separator();
            if (filesContext->toCut.contains(fEntry.id)) {
                ImGui::TextDisabled(fEntry.name.c_str());
            } else {
                ImGui::Text(fEntry.name.c_str());
            }
        }
        ImGui::EndChild();
    }

    void FilesPanel::hotkeys() {
        if (!isSomethingHovered) {
            return;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) && !filesContext->selected.empty()) {
            openSelected();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !filesContext->selected.empty()) {
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
        for (auto &entry: filesContext->toCut) {
            // var children = filesRepository.parentChildren.get(currentDirectory);
            // if (!children.contains(key)) {
            //     var previousParentId = filesRepository.childParent.get(key);
            //     filesRepository.parentChildren.get(previousParentId).remove(key);
            //
            //     children.add(key);
            //     filesRepository.childParent.put(key, currentDirectory);
            // }
        }
        filesContext->toCut.clear();
    }

    void FilesPanel::openSelected() const {
        for (auto &id: filesContext->selected) {
            openResource(*id.second);
        }
    }

    void FilesPanel::cutSelected() const {
        filesContext->toCut.clear();
        filesContext->toCut = filesContext->selected;
    }

    void FilesPanel::selectAll() const {
        for (auto &entry: filesContext->currentDirectory->children) {
            filesContext->selected[entry.id] = &entry;
        }
    }

    void FilesPanel::deleteSelected() {
        // filesService.deleteSelected(selected.keySet());
    }

    void FilesPanel::onClick(FileEntry &root) const {
        if (root.isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                filesContext->selected.clear();
            }
            if (filesContext->selected.contains(root.id) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                filesContext->selected.erase(root.id);
            } else {
                filesContext->selected[root.id] = &root;
            }
        }
        if (root.isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            openResource(root);
        }
    }

    void FilesPanel::openResource(FileEntry &root) const {
        if (root.type == EntryType::DIRECTORY) {
            filesContext->setCurrentDirectory(&root);
            filesContext->selected.clear();
        } else {
            // OPEN FILE
        }
    }
} // Metal
