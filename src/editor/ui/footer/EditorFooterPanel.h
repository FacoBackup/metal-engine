#ifndef METAL_ENGINE_EDITORFOOTERPANEL_H
#define METAL_ENGINE_EDITORFOOTERPANEL_H

#include "editor/dto/FileEntryDTO.h"
#include "editor/ui/abstract/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;
    class ThemeService;
    class FilesService;
    class AsyncTaskPanel;

    class EditorFooterPanel final : public AbstractPanel {
        EditorRepository *editorRepository = nullptr;
        ThemeService *themeService = nullptr;
        FilesService *filesService = nullptr;
        std::shared_ptr<AsyncTaskPanel> asyncTaskPanel = nullptr;

        std::vector<FileEntryDTO> levelCache{};

        void renderShortcuts();

        void framerate();

        void renderLevelPopup();

        void refreshLevelCache();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"ThemeService", &themeService},
                {"FilesService", &filesService}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}

#endif
