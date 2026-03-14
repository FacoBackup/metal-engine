#ifndef FILESPANEL_H
#define FILESPANEL_H

#include <functional>
#include <map>

#include "FilesContext.h"
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

    class FilesPanel : public AbstractDockPanel {
    protected:
        FilesContext filesContext{nullptr};
        FilePreviewPanel *previewPanel = nullptr;
        AbstractPanel *filesHeader = nullptr;
        AbstractPanel *filesListPanel = nullptr;
        float previewWidth = 200.0f;

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
                {"FileImporterService", fileImporterService},
                {"EditorRepository", editorRepository},
                {"SceneImporterService", sceneImporterService},
                {"FilesService", filesService},
                {"NotificationService", notificationService},
                {"WorldRepository", worldRepository},
                {"VoxelService", voxelService}
            };
        }

        virtual bool renderPreview() {
            return true;
        }

        virtual std::string getActionLabel();

        virtual std::function<void()> onAction();

        void onInitialize() override;

        void onSync() override;

        static void SetIconPos(const char *text);

        void pasteSelected();

        void openSelected();

        void cutSelected();

        void selectAll();

        void deleteSelected() const;

        virtual EntryType::EntryType getTypeFilter() {
            return EntryType::NONE;
        }

        virtual void openResource(FSEntry *root);

        virtual bool renderHeader() { return true; }
    };
} // Metal

#endif //FILESPANEL_H
