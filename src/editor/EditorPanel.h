#ifndef METAL_ENGINE_EDITORPANEL_H
#define METAL_ENGINE_EDITORPANEL_H

#include "imgui.h"
#include "abstract/AbstractPanel.h"

namespace Metal {
    class DockService;
    class ThemeService;

    class EditorPanel final : public AbstractPanel {
        static int FLAGS;
        static const char *NAME;
        static const char *NAME_HEADER;
        static const char *NAME_FOOTER;
        static ImVec2 CENTER;
        static float HEADER_HEIGHT;
        static float FOOTER_HEIGHT;
        ImGuiID windowId = 0;
        AbstractPanel *headerPanel = nullptr;
        AbstractPanel *footerPanel = nullptr;
        AbstractPanel *notificationsPanel = nullptr;
        AbstractPanel *fileImportModalPanel = nullptr;

        DockService *dockService = nullptr;
        ThemeService *themeService = nullptr;

        static void SetWindowStyle();

        void renderDockSpaces();

        void renderHeader(const ImGuiViewport *viewport);

        void renderFooter(const ImGuiViewport *viewport);

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"DockService", dockService},
                {"ThemeService", themeService}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}


#endif
