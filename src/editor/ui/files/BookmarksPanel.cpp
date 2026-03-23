#include "BookmarksPanel.h"
#include "../../repository/EditorRepository.h"
#include "../../service/FilesService.h"
#include "editor/service/ThemeService.h"
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "editor/dto/FSEntry.h"
#include <filesystem>
#include <algorithm>
#include <imgui.h>

namespace Metal {
    void BookmarksPanel::onSync() {
        if (UIUtil::Accordion(id, Icons::bookmark + " Bookmarks", isOpen, themeService->palette3)) {
            if (UIUtil::ButtonSimple(Icons::add + " Add Bookmark", 0, 0)) {
                if (filesContext.selected.empty()) {
                    if (filesContext.currentDirectory) {
                        const std::string path = filesContext.currentDirectory->absolutePath;
                        if (std::find(editorRepository->bookmarks.begin(), editorRepository->bookmarks.end(), path) == editorRepository->bookmarks.end()) {
                            editorRepository->bookmarks.push_back(path);
                        }
                    }
                } else {
                    for (auto const& [selectionId, entry] : filesContext.selected) {
                        if (std::find(editorRepository->bookmarks.begin(), editorRepository->bookmarks.end(), entry->absolutePath) == editorRepository->bookmarks.end()) {
                            editorRepository->bookmarks.push_back(entry->absolutePath);
                        }
                    }
                }
            }

            ImGui::Separator();

            for (size_t i = 0; i < editorRepository->bookmarks.size(); ++i) {
                const auto& path = editorRepository->bookmarks[i];
                std::filesystem::path p(path);
                std::string name = p.filename().string();
                if (name.empty()) name = path; // Fallback for root

                bool isSelected = (filesContext.currentDirectory && filesContext.currentDirectory->absolutePath == path);

                if (ImGui::Selectable((Icons::folder + " " + name + "##" + std::to_string(i)).c_str(), isSelected)) {
                    std::shared_ptr<FSEntry> entry = filesService->GetEntry(path);
                    if (entry) {
                        filesContext.setCurrentDirectory(entry);
                        filesService->GetEntries(entry);
                        filesContext.selected.clear();
                    }
                }

                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Remove Bookmark")) {
                        editorRepository->bookmarks.erase(editorRepository->bookmarks.begin() + i);
                        ImGui::EndPopup();
                        break;
                    }
                    ImGui::EndPopup();
                }
            }
            UIUtil::EndAccordion();
        }
    }
}
