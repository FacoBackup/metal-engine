#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include <functional>

#include "../../abstract/AbstractPanel.h"
#include <imgui.h>

namespace Metal {
    class HistoryService;
    class ThemeService;
    class WindowService;
    class EngineContext;
    struct DirectoryService;
    class DockService;

    class EditorHeaderPanel final : public AbstractPanel {
        WindowService *windowService = nullptr;
        EngineContext *engineContext = nullptr;
        DirectoryService *directoryService = nullptr;
        HistoryService *historyService = nullptr;
        ThemeService *themeService = nullptr;
        DockService *dockService = nullptr;

        float menuBarHeight = 0;

        void renderMenu(const char *label, std::function<void()> itemsFunc);
        
        void renderDockAdders();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WindowService", &windowService},
                {"EngineContext", &engineContext},
                {"DirectoryService", &directoryService},
                {"ThemeService", &themeService},
                {"historyService", &historyService},
                {"DockService", &dockService}
            };
        }

        void onSync() override;
    };
} // Metal

#endif
