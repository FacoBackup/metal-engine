#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include "../../abstract/AbstractPanel.h"
#include <imgui.h>

namespace Metal {
    class GLFWContext;
    class EngineContext;
    struct DirectoryService;

    class EditorHeaderPanel final : public AbstractPanel {
        GLFWContext *glfwContext = nullptr;
        EngineContext *engineContext = nullptr;
        DirectoryService *directoryService = nullptr;

        ImVec2 dragStartMousePos;
        int dragStartWindowX = 0;
        int dragStartWindowY = 0;
        bool isDragging = false;

        double dragStartGlobalMouseX = 0;
        double dragStartGlobalMouseY = 0;
        int lastWindowX = 0;
        int lastWindowY = 0;

        ImVec2 dragHandleScreenPos;

        void renderFileTab();

        void renderWindowControls();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"GLFWContext", &glfwContext},
                {"EngineContext", &engineContext},
                {"DirectoryService", &directoryService}
            };
        }

        void onSync() override;
    };
} // Metal

#endif
