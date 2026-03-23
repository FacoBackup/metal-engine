#ifndef ABSTRACTDIRECTORYPANEL_H
#define ABSTRACTDIRECTORYPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "FilesContext.h"
#include "editor/dto/FSEntry.h"
#include "editor/ui/UIUtil.h"
#include "../../service/FileImporterService.h"
#include "common/FileExtensions.h"
#include <functional>

#include <algorithm>
#include <string>

namespace Metal {
    class FilesService;

    class AbstractDirectoryPanel : public AbstractPanel {
    protected:
        FilesContext &filesContext;
        std::function<void(std::shared_ptr<FSEntry>)> onDoubleClick;
        FilesService *filesService = nullptr;
        FileImporterService *fileImporterService = nullptr;

        bool matchesSearch(const std::shared_ptr<FSEntry> &entry) const {
            if (std::strlen(filesContext.searchQuery) == 0) return true;
            std::string query = filesContext.searchQuery;
            std::string name = entry->name;
            std::ranges::transform(query, query.begin(), ::tolower);
            std::ranges::transform(name, name.begin(), ::tolower);
            return name.find(query) != std::string::npos;
        }

        static void renderInfoTooltip(const std::shared_ptr<FSEntry> &entry) {
            if (ImGui::BeginItemTooltip()) {
                ImGui::Text("Name: %s", entry->name.c_str());
                ImGui::Text("Path: %s", entry->absolutePath.c_str());
                const std::string typeLabel = entry->isDirectory ? "Directory" : UIUtil::GetExtensionLabel(entry->extension);
                ImGui::Text("Type: %s", typeLabel.c_str());
                if (!entry->isDirectory) {
                    ImGui::Text("Size: %s", entry->formattedSize.c_str());
                }
                ImGui::Text("Date: %s", entry->formattedDate.c_str());
                ImGui::EndTooltip();
            }
        }

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"FilesService", &filesService},
                {"FileImporterService", &fileImporterService}
            };
        }

        explicit AbstractDirectoryPanel(FilesContext &filesContext, std::function<void(std::shared_ptr<FSEntry>)> onDoubleClick)
            : filesContext(filesContext), onDoubleClick(std::move(onDoubleClick)) {
        }
    };
}

#endif //ABSTRACTDIRECTORYPANEL_H
