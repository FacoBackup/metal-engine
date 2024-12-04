#ifndef METAL_ENGINE_DOCKSPACEPANEL_H
#define METAL_ENGINE_DOCKSPACEPANEL_H

#include "../../common/AbstractPanel.h"

namespace Metal {
    class AbstractDockPanel;

    class DockSpacePanel : public AbstractPanel {
        static const int FLAGS = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
        static const ImVec2 DEFAULT;
        static const ImVec2 MAX_SIZE;
        static const ImVec2 PIVOT;
        static const float FRAME_SIZE;
        static const ImVec2 MIN_SIZE;

        ImVec2 padding{DEFAULT.x, DEFAULT.y};
        ImVec2 position{DEFAULT.x, DEFAULT.y};
        glm::vec2 size{};
        ImVec2 sizeInternal{DEFAULT.x, DEFAULT.y};
        bool isDownDirection;
        bool sizeInitialized = false;
        bool isNotCenter;
        int stylePushCount;
        DockSpacePanel *mainWindow;
        DockDTO *dock;
        AbstractDockPanel *view;
        ImVec2 headerPadding{0, 3};

    public:
        explicit DockSpacePanel(DockSpacePanel *mainWindow, DockDTO *dock) : mainWindow(mainWindow), dock(dock) {}

        void onInitialize() override;

        void initializeView();

        void onSync() override;

        AbstractDockPanel *getView();

        void renderHeader();

        void beforeWindow();

        ImVec2 &getPosition();

        ImVec2 &getSize();
    };

}

#endif
