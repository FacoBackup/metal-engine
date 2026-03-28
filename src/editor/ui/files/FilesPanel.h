#ifndef FILESPANEL_H
#define FILESPANEL_H

#include <functional>
#include <map>

#include "FilesContext.h"
#include "ListDirectoryPanel.h"
#include "CardDirectoryPanel.h"
#include "NavigationPanel.h"
#include "../docks/AbstractDockPanel.h"
#include "../../dto/ImportSettingsDTO.h"

namespace Metal {
    struct FSEntry;
    class FilePreviewPanel;
    class FormPanel;
    class FileImporterService;
    struct EditorRepository;
    class SceneImporterService;
    class FilesService;
    class NotificationService;
    struct WorldRepository;
    class VoxelService;
    class ListDirectoryPanel;
    class CardDirectoryPanel;
    class NavigationPanel;

    class FilesPanel final : public AbstractDockPanel {
    protected:
        FilesContext filesContext{nullptr, nullptr};
        std::shared_ptr<AbstractPanel> filesHeader = nullptr;
        std::shared_ptr<ListDirectoryPanel> listDirectoryPanel = nullptr;
        std::shared_ptr<CardDirectoryPanel> cardDirectoryPanel = nullptr;
        std::shared_ptr<NavigationPanel> navigationPanel = nullptr;

        FileImporterService *fileImporterService = nullptr;
        EditorRepository *editorRepository = nullptr;
        SceneImporterService *sceneImporterService = nullptr;
        FilesService *filesService = nullptr;
        NotificationService *notificationService = nullptr;
        WorldRepository *worldRepository = nullptr;
        VoxelService *voxelService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"FileImporterService", &fileImporterService},
                {"EditorRepository", &editorRepository},
                {"SceneImporterService", &sceneImporterService},
                {"FilesService", &filesService},
                {"NotificationService", &notificationService},
                {"WorldRepository", &worldRepository},
                {"VoxelService", &voxelService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        static void SetIconPos(const char *text);

        void pasteSelected();

        void openSelected();

        void cutSelected();

        void selectAll();
        
        void deleteSelected() const;
    };
} // Metal

#endif //FILESPANEL_H
