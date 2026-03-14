#ifndef METAL_ENGINE_FILEIMPORTMODALPANEL_H
#define METAL_ENGINE_FILEIMPORTMODALPANEL_H
#include "../abstract/AbstractPanel.h"

namespace Metal {
    class FilesService;
    class FormPanel;
    struct EditorRepository;
    class FileImporterService;
    class NotificationService;

    class FileImportModalPanel final : public AbstractPanel {
        FormPanel *formPanel = nullptr;
        bool isFirst = false;

        EditorRepository *editorRepository = nullptr;
        FileImporterService *fileImporterService = nullptr;
        NotificationService *notificationService = nullptr;
        FilesService *filesService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", editorRepository},
                {"FileImporterService", fileImporterService},
                {"NotificationService", notificationService},
                {"FilesService", filesService}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //METAL_ENGINE_FILEIMPORTMODALPANEL_H
