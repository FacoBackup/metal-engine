#ifndef METAL_ENGINE_DOCKSPACEPANEL_H
#define METAL_ENGINE_DOCKSPACEPANEL_H

#include <imgui.h>
#include <glm/vec2.hpp>
#include <unordered_map>

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct DockDTO;
    struct DockSpace;
    class AbstractDockPanel;
    struct EditorRepository;
    class ThemeService;

    class DockSpacePanel final : public AbstractPanel {
        ImVec2 padding{DEFAULT.x, DEFAULT.y};
        ImVec2 position{DEFAULT.x, DEFAULT.y};
        glm::vec2 size{};
        ImVec2 sizeInternal{DEFAULT.x, DEFAULT.y};
        bool isDownDirection = false;
        bool sizeInitialized = false;
        int stylePushCount = 0;
        int styleColorPushCount = 0;
        DockSpacePanel *mainWindow = nullptr;
        DockDTO *dock = nullptr;
        AbstractDockPanel *view = nullptr;
        std::unordered_map<int, AbstractDockPanel *> views{};
        ImVec2 headerPadding{0, 3};
        float headerHeight = 25.0f;

        EditorRepository *editorRepository = nullptr;
        ThemeService *themeService = nullptr;

        DockSpace *getSelectedDockSpace() const;

        bool hasDockSpace(int index) const;

        void renderCustomHeader();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"ThemeService", &themeService}
            };
        }

        static constexpr int FLAGS = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
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

        void handleShortcut() const;

        void onSync() override;

        [[nodiscard]] AbstractDockPanel *getView() const;

        void beforeWindow() const;

        ImVec2 &getPosition();

        ImVec2 &getSize();
    };
}

#endif
