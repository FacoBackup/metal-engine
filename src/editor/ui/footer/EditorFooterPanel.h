#ifndef METAL_ENGINE_EDITORFOOTERPANEL_H
#define METAL_ENGINE_EDITORFOOTERPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;
    class ThemeService;
    class AsyncTaskPanel;

    class EditorFooterPanel final : public AbstractPanel {
        EditorRepository *editorRepository = nullptr;
        ThemeService *themeService = nullptr;
        std::shared_ptr<AsyncTaskPanel> asyncTaskPanel = nullptr;

        void renderShortcuts();
        void framerate();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"ThemeService", &themeService}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}

#endif
