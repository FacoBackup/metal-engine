#include "FilesHeaderPanel.h"

#include "../../enum/EntryType.h"
#include "../../dto/FSEntry.h"
#include "../../../common/Icons.h"
#include "../../../ApplicationContext.h"
#include "../../service/FilesService.h"
#include "../../util/UIUtil.h"

namespace Metal {
    void FilesHeaderPanel::onSync() {
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesService->CreateDirectory(filesContext.currentDirectory);
            filesService->GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Create folder");
        ImGui::SameLine();

        if (filesContext.currentDirectory->parent != nullptr) {
            if (UIUtil::ButtonSimple(Icons::arrow_upward, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                     UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                filesContext.setCurrentDirectory(filesContext.currentDirectory->parent);
            }
            UIUtil::RenderTooltip("Go to parent folder");
        }

        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::refresh, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesService->GetEntries(filesContext.currentDirectory);
        }
        UIUtil::RenderTooltip("Refresh");

        ImGui::SameLine();

        ImGui::Text(filesContext.pathToCurrentDirectory.c_str());
        UIUtil::DynamicSpacing(212);

        ImGui::SetNextItemWidth(112);
        const auto &currentDTO = EntryType::GetDTO(filesContext.filterType);
        std::string filterLabel = currentDTO.icon + " " + currentDTO.name;
        if (ImGui::Button((filterLabel + id + "filter").c_str(), ImVec2(100, 0))) {
            ImGui::OpenPopup((id + "FilterPopup").c_str());
        }
        if (ImGui::BeginPopup((id + "FilterPopup").c_str())) {
            for (const auto &dto: EntryType::ENTRIES) {
                if (ImGui::MenuItem((dto.icon + " " + dto.name).c_str())) {
                    filesContext.filterType = dto.type;
                }
            }
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        if (UIUtil::ButtonSimple(actionLabel + id, 100,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            action();
        }
    }
} // Metal
