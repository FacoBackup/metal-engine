#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include <functional>

#include "../../abstract/AbstractPanel.h"
#include <imgui.h>

namespace Metal {
    class ThemeService;
    class GLFWContext;
    class EngineContext;
    struct DirectoryService;

    class EditorHeaderPanel final : public AbstractPanel {
        GLFWContext *glfwContext = nullptr;
        EngineContext *engineContext = nullptr;
        DirectoryService *directoryService = nullptr;
        ThemeService *themeService = nullptr;

        float menuBarHeight = 0;

        void renderFileTab();

        void renderMenu(const char *label, std::function<void()> itemsFunc);

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"GLFWContext", &glfwContext},
                {"EngineContext", &engineContext},
                {"DirectoryService", &directoryService},
                {"ThemeService", &themeService}
            };
        }

        void onSync() override;
    };
} // Metal

#endif
