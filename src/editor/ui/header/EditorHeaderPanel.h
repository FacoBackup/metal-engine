#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include <functional>
#include "../../../core/WindowService.h"

#include "editor/ui/abstract/AbstractPanel.h"
#include <imgui.h>

namespace Metal {
    class HistoryService;
    class ThemeService;
    class WindowService;
    class EngineContext;
    class TextureService;
    class ImGuiService;
    class DirectoryService;
    class SnapshotService;
    class DockService;
    struct EditorRepository;

    class EditorHeaderPanel final : public AbstractPanel {
        WindowService *windowService = nullptr;
        EngineContext *engineContext = nullptr;
        DirectoryService *directoryService = nullptr;
        SnapshotService *snapshotService = nullptr;
        HistoryService *historyService = nullptr;
        ThemeService *themeService = nullptr;
        DockService *dockService = nullptr;
        EditorRepository *editorRepository = nullptr;
        TextureService *textureService = nullptr;
        ImGuiService *imguiService = nullptr;

        float menuBarHeight = 0;

        void renderDockAdders(WindowService::WindowControlRects &rects) const;

        void renderLogo();

        void renderProjectName(WindowService::WindowControlRects &rects);

        void renderPlayStopButtons(WindowService::WindowControlRects &rects);

        void renderHistoryPopup();

        void renderHeaderMenuPopup();

        static void RenderWindowControl(const std::string &icon, const char *id, float xPos, float width,
                                        ImU32 hoverColor, std::function<void()> action);

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WindowService", &windowService},
                {"EngineContext", &engineContext},
                {"DirectoryService", &directoryService},
                {"SnapshotService", &snapshotService},
                {"ThemeService", &themeService},
                {"historyService", &historyService},
                {"DockService", &dockService},
                {"EditorRepository", &editorRepository},
                {"TextureService", &textureService},
                {"ImguiService", &imguiService}
            };
        }

        void onSync() override;
    };
} // Metal

#endif
