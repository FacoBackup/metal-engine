#ifndef METAL_ENGINE_DOCKSPACEPANEL_H
#define METAL_ENGINE_DOCKSPACEPANEL_H

#include <imgui.h>
#include <iosfwd>
#include <glm/vec2.hpp>

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct DockViewDTO;
    struct DockDTO;
    class AbstractDockPanel;

    class DockSpacePanel final : public AbstractPanel {
        ImVec2 position{DEFAULT.x, DEFAULT.y};
        glm::vec2 size{};
        ImVec2 sizeInternal{DEFAULT.x, DEFAULT.y};
        bool isDownDirection = false;
        bool sizeInitialized = false;
        int stylePushCount = 0;
        DockSpacePanel *mainWindow = nullptr;
        DockDTO *dock = nullptr;
        ImVec2 headerPadding{0, 3};

    public:
        static const ImVec2 DEFAULT;
        static const ImVec2 MAX_SIZE;
        static const ImVec2 PIVOT;
        static const float FRAME_SIZE;
        static const ImVec2 MIN_SIZE;

        explicit DockSpacePanel(DockSpacePanel *mainWindow, DockDTO *dock) : mainWindow(mainWindow), dock(dock) {
        }

        [[nodiscard]] DockDTO *getDock() const {
            return dock;
        }

        void onInitialize() override;

        void initializeView();

        void onSync() override;

        void renderTabs();

        void renderView(DockViewDTO &viewDTO);

        void beforeWindow() const;

        ImVec2 &getPosition();

        ImVec2 &getSize();
    };
}

#endif
