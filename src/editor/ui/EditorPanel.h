#ifndef METAL_ENGINE_EDITORPANEL_H
#define METAL_ENGINE_EDITORPANEL_H

#include "imgui.h"
#include "editor/ui/abstract/AbstractPanel.h"

namespace Metal {
    class DockService;
    class ThemeService;
    class EditorRepository;

    class EditorPanel final : public AbstractPanel {
        static int FLAGS;
        static const char *NAME;
        static const char *NAME_HEADER;
        static const char *NAME_FOOTER;
        static ImVec2 CENTER;
        ImGuiID windowId = 0;
        std::shared_ptr<AbstractPanel> headerPanel = nullptr;
        std::shared_ptr<AbstractPanel> footerPanel = nullptr;
        std::shared_ptr<AbstractPanel> notificationsPanel = nullptr;

        DockService *dockService = nullptr;
        ThemeService *themeService = nullptr;
        EditorRepository *editorRepository = nullptr;

        static void SetWindowStyle(const ImVec2 &padding);

        void renderDockSpaces();

    public:
        static float HEADER_HEIGHT;
        static float FOOTER_HEIGHT;

        std::vector<Dependency> getDependencies() override {
            return {
                {"DockService", &dockService},
                {"ThemeService", &themeService},
                {"EditorRepository", &editorRepository}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}


#endif
