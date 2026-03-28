#include "AbstractDirectoryPanel.h"
#include "imgui.h"
#include "editor/dto/FSEntry.h"
#include "../../dto/ImportSettingsDTO.h"
#include "../../service/FilesService.h"
#include "../../service/FileImporterService.h"
#include "common/ILoader.h"
#include "common/AbstractImporter.h"
#include "common/Icons.h"
#include "common/FileExtensions.h"
#include "ApplicationContext.h"
#include <algorithm>

namespace Metal {
    AbstractDirectoryPanel::AbstractDirectoryPanel(FilesContext &filesContext)
        : filesContext(filesContext) {
    }

    const std::string &AbstractDirectoryPanel::getEntryIcon(std::shared_ptr<FSEntry> entry) const {
        if (entry->isDirectory) return Icons::folder;

        const std::string &extension = entry->extension;
        for (const auto *info: Metal::FileExtensions::all) {
            if (info->extension == extension) return info->icon;
        }

        return Icons::insert_drive_file;
    }

    bool AbstractDirectoryPanel::matchesSearch(std::shared_ptr<FSEntry> entry) const {
        if (std::strlen(filesContext.searchQuery) == 0) return true;
        std::string query = filesContext.searchQuery;
        std::string name = entry->name;
        std::ranges::transform(query, query.begin(), ::tolower);
        std::ranges::transform(name, name.begin(), ::tolower);
        return name.find(query) != std::string::npos;
    }

    std::string AbstractDirectoryPanel::getExtensionLabel(const std::string &extension) {
        for (const auto *info: Metal::FileExtensions::all) {
            if (info->extension == extension) return info->typeLabel;
        }

        return "File";
    }

    void AbstractDirectoryPanel::renderInfoTooltip(std::shared_ptr<FSEntry> entry) {
        if (ImGui::BeginItemTooltip()) {
            ImGui::Text("Name: %s", entry->name.c_str());
            ImGui::Text("Path: %s", entry->absolutePath.c_str());
            const std::string typeLabel = entry->isDirectory ? "Directory" : getExtensionLabel(entry->extension);
            ImGui::Text("Type: %s", typeLabel.c_str());
            if (!entry->isDirectory) {
                ImGui::Text("Size: %s", entry->formattedSize.c_str());
            }
            ImGui::Text("Date: %s", entry->formattedDate.c_str());
            ImGui::EndTooltip();
        }
    }

    std::vector<Dependency> AbstractDirectoryPanel::getDependencies() {
        return {
            {"FilesService", &filesService},
            {"FileImporterService", &fileImporterService}
        };
    }

    void AbstractDirectoryPanel::openResource(std::shared_ptr<FSEntry> root) const {
        if (root == nullptr) {
            return;
        }

        if (root->isDirectory) {
            filesService->GetEntries(root);
            filesContext.currentDirectory = root;
            filesContext.selected.clear();
            return;
        }

        fileImporterService->importFile(root,
                                        std::make_shared<ImportSettingsDTO>());
    }
}
