#include "DockSpace.h"
#include "../../common/interface/Icons.h"
#include "../../context/editor/dock-spaces/console/ConsolePanel.h"
#include "../../context/editor/dock-spaces/files/FilesPanel.h"
#include "../../context/editor/dock-spaces/inspector/InspectorPanel.h"
#include "../../context/editor/dock-spaces/metrics/MetricsPanel.h"
#include "../../context/editor/dock-spaces/viewport/ViewportPanel.h"
#include "../../context/editor/dock-spaces/hierarchy/HierarchyPanel.h"
#include "../../context/editor/dock-spaces/canvas/CanvasPanel.h"

#define CB(clazz) []() { return new clazz; }

namespace Metal {
    DockSpace DockSpace::VIEWPORT{0, "Viewport", Icons::ipublic, CB(ViewportPanel)};
    DockSpace DockSpace::INSPECTOR{1, "Inspector", Icons::search, CB(InspectorPanel)};
    DockSpace DockSpace::HIERARCHY{2, "Hierarchy", Icons::account_tree, CB(HierarchyPanel)};
    DockSpace DockSpace::CONSOLE{3, "Console", Icons::terminal, CB(ConsolePanel)};
    DockSpace DockSpace::FILES{4, "Files", Icons::folder_open, CB(FilesPanel)};
    DockSpace DockSpace::METRICS{5, "Metrics", Icons::bar_chart, CB(MetricsPanel)};
    DockSpace DockSpace::CANVAS{6, "Scripting canvas", Icons::code, CB(CanvasPanel)};
    const char *DockSpace::OPTIONS = "Viewport\0Inspector\0Hierarchy\0Console\0Files\0Metrics\0Scripting";

    DockSpace *DockSpace::GetOption(const unsigned int selected) {
        switch (selected) {
            case 0:
                return &VIEWPORT;
            case 1:
                return &INSPECTOR;
            case 2:
                return &HIERARCHY;
            case 3:
                return &CONSOLE;
            case 4:
                return &FILES;
            case 5:
                return &METRICS;
            case 6:
                return &CANVAS;
            default:
                return nullptr;
        }
    }

    int DockSpace::GetOptionCount() {
        return 6;
    }
}
