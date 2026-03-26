#ifndef ABSTRACTDIRECTORYPANEL_H
#define ABSTRACTDIRECTORYPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "FilesContext.h"
#include "editor/dto/FSEntry.h"
#include "editor/ui/UIUtil.h"
#include "../../service/FileImporterService.h"
#include "common/FileExtensions.h"
#include "common/Icons.h"
#include "common/ILoader.h"
#include "common/AbstractImporter.h"
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

        bool canInteract(const std::shared_ptr<FSEntry> &entry) const {
            if (entry->isDirectory) return true;
            const std::string &ext = entry->extension;
            for (auto *loader: ctx->getSingletons<ILoader>()) {
                if (loader->isCompatible(ext)) return true;
            }
            for (auto *importer: ctx->getSingletons<AbstractImporter>()) {
                if (importer->isCompatible(entry->absolutePath)) return true;
            }
            return false;
        }

        std::string getEntryIcon(const std::shared_ptr<FSEntry> &entry) const {
            if (entry->isDirectory) return Icons::folder;
            if (!canInteract(entry)) return Icons::close;
            
            const std::string &extension = entry->extension;
            if (extension == EXT_SCENE) return Icons::image;
            if (extension == EXT_MESH) return Icons::view_in_ar;
            if (extension == EXT_PNG || extension == EXT_JPG || extension == EXT_JPEG || extension == EXT_TGA) return Icons::texture;
            if (extension == EXT_SVO || extension == EXT_VDB) return Icons::view_agenda;
            if (extension == EXT_OBJ || extension == EXT_FBX || extension == EXT_GLTF || extension == EXT_GLB) return Icons::image;
            return Icons::insert_drive_file;
        }

        bool matchesSearch(const std::shared_ptr<FSEntry> &entry) const {
            if (std::strlen(filesContext.searchQuery) == 0) return true;
            std::string query = filesContext.searchQuery;
            std::string name = entry->name;
            std::ranges::transform(query, query.begin(), ::tolower);
            std::ranges::transform(name, name.begin(), ::tolower);
            return name.find(query) != std::string::npos;
        }

        std::string getExtensionLabel(const std::string &extension) const {
            if (extension == EXT_SCENE) return "Scene";
            if (extension == EXT_MESH) return "Mesh";
            if (extension == EXT_PNG || extension == EXT_JPG || extension == EXT_JPEG || extension == EXT_TGA) return "Texture";
            if (extension == EXT_SVO) return "Volume";
            if (extension == EXT_VDB) return "OpenVDB";
            if (extension == EXT_OBJ || extension == EXT_FBX || extension == EXT_GLTF || extension == EXT_GLB) return "Scene Source";
            return "File";
        }

        static void renderInfoTooltip(const std::shared_ptr<FSEntry> &entry, const AbstractDirectoryPanel* panel) {
            if (ImGui::BeginItemTooltip()) {
                ImGui::Text("Name: %s", entry->name.c_str());
                ImGui::Text("Path: %s", entry->absolutePath.c_str());
                const std::string typeLabel = entry->isDirectory ? "Directory" : panel->getExtensionLabel(entry->extension);
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
