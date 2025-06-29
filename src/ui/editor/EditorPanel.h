#ifndef METAL_ENGINE_EDITORPANEL_H
#define METAL_ENGINE_EDITORPANEL_H

#include "imgui.h"
#include "abstract/AbstractPanel.h"
#include "dock-spaces/header/EditorHeaderPanel.h"

namespace Metal {
    class EditorPanel final : public AbstractPanel {
        static int FLAGS;
        static const char *NAME;
        static const char *NAME_HEADER;
        static ImVec2 CENTER;
        static float HEADER_HEIGHT;
        ImGuiID windowId = 0;
        EditorHeaderPanel headerPanel;

        static void SetWindowStyle();

        void renderDockSpaces();

        void renderHeader(const ImGuiViewport *viewport);

    public:
        void onInitialize() override;

        void onSync() override;
    };
}


#endif
