#include "FilesPanel.h"


#include "FilesHeader.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../util/FilesUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/mesh/SceneData.h"
#include "../../../../util/UIUtil.h"
#include "../../../../dto/file//FileEntry.h"
#include "FilesContext.h"
#include "../../../../util/FileDialogUtil.h"
#include "../../../../enum/LevelOfDetail.h"

namespace Metal {
    std::string FilesPanel::getActionLabel() {
        return Icons::file_open + "Import file";
    }

    std::function<void()> FilesPanel::onAction() {
        return [this]() {
            auto files = FileDialogUtil::PickFiles({{"Mesh", "fbx,gltf,obj,glb"}, {"Image", "png,jpg,jpeg"}});
            for (const std::string &file: files) {
                if (context->meshImporter.isCompatible(file)) {
                    context->meshImporter.importScene(filesContext.currentDirectory->absolutePath,
                                                                         file);
                } else if (context->textureImporter.isCompatible(file)) {
                    context->textureImporter.importTexture(
                        filesContext.currentDirectory->absolutePath, file);
                }
            }
            FilesService::GetEntries(filesContext.currentDirectory);
        };
    }

    void FilesPanel::onInitialize() {
        filesContext.setCurrentDirectory(context->filesService.getRoot());
        appendChild(new FilesHeader(filesContext, getActionLabel(), onAction()));
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
        onSyncChildren();
        ImGui::Separator();
        if (ImGui::BeginChild(id.c_str())) {
            isSomethingHovered = ImGui::IsWindowHovered();
            if (ImGui::IsWindowFocused()) {
                filesContext.selected.clear();
            }
            contextMenu();
            handleDrag();
            hotkeys();

            float size = std::floor(ImGui::GetWindowSize().x / CARD_SIZE) * CARD_SIZE - CARD_SIZE;
            int rowIndex = 1;
            for (auto &child: filesContext.currentDirectory->children) {
                if (filesContext.filterType == EntryType::NONE || child->type == filesContext.filterType) {
                    const bool isSelected = filesContext.selected.contains(child->getId()) || child->isHovered && onDrag
                                            !=
                                            nullptr;
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, isSelected
                                                                ? context->editorRepository.accent
                                                                : context->themeService.palette0);
                    renderItem(child);
                    ImGui::PopStyleColor();

                    if (rowIndex * CARD_SIZE < size) {
                        ImGui::SameLine();
                        rowIndex++;
                    } else {
                        rowIndex = 0;
                    }
                }
            }
            trackDrag();
        }

        ImGui::EndChild();
    }

    void FilesPanel::SetIconPos(const char *text) {
        const auto windowWidth = ImGui::GetWindowSize().x;
        const auto windowHeight = ImGui::GetWindowSize().y;
        const auto textWidth = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::SetCursorPosY((windowHeight - LARGE_FONT_SIZE * 2) * 0.5f);
    }

    void FilesPanel::renderItem(FileEntry *root) {
        UIUtil::AUX_VEC2.x = CARD_SIZE;
        UIUtil::AUX_VEC2.y = CARD_SIZE + 15;
        if (ImGui::BeginChild(root->getId().c_str(), UIUtil::AUX_VEC2, ImGuiChildFlags_Border)) {
            root->isHovered = ImGui::IsWindowHovered();
            isSomethingHovered = isSomethingHovered || root->isHovered;
            onClick(root);
            handleDragDrop(root);
            ImGui::PushFont(context->guiContext.getLargeIconsFont());

            if (root->type == EntryType::DIRECTORY) {
                SetIconPos(Icons::folder.c_str());
                ImGui::TextColored(UIUtil::DIRECTORY_COLOR, Icons::folder.c_str());
            } else if (root->type == EntryType::TEXTURE) {
                auto *texture = context->streamingRepository.streamTexture(
                    root->getId(), LevelOfDetail::LOD_3);
                if (texture != nullptr) {
                    context->guiContext.renderImage(texture, CARD_SIZE - 8, CARD_SIZE - 22);
                }
            } else {
                std::string icon = UIUtil::GetFileIcon(root->type);
                SetIconPos(icon.c_str());
                ImGui::Text(icon.c_str());
            }
            ImGui::PopFont();
            UIUtil::AUX_VEC2.x = 0;
            UIUtil::AUX_VEC2.y = ImGui::GetContentRegionAvail().y - TEXT_OFFSET;
            ImGui::Dummy(UIUtil::AUX_VEC2);
            ImGui::Separator();
            if (filesContext.toCut.contains(root->getId())) {
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
            filesContext.selected.insert({entry->getId(), entry});
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
