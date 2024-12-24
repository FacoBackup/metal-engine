#include "DockSpace.h"
#include "../../common/interface/Icons.h"
#include "../../context/editor/dock-spaces/console/ConsolePanel.h"
#include "../../context/editor/dock-spaces/files/FilesPanel.h"
#include "../../context/editor/dock-spaces/inspector/InspectorPanel.h"
#include "../../context/editor/dock-spaces/metrics/MetricsPanel.h"
#include "../../context/editor/dock-spaces/viewport/ViewportPanel.h"
#include "../../context/editor/dock-spaces/hierarchy/HierarchyPanel.h"

#define CB(clazz) []() { return new clazz; }

namespace Metal {
    DockSpace DockSpace::VIEWPORT{-1, "Viewport", Icons::ipublic, 0, 0,CB(ViewportPanel)};
    DockSpace DockSpace::INSPECTOR{0, "Inspector", Icons::search, 4, 4,CB(InspectorPanel)};
    DockSpace DockSpace::HIERARCHY{1, "Hierarchy", Icons::account_tree, 4, 4, CB(HierarchyPanel)};
    DockSpace DockSpace::CONSOLE{2, "Console", Icons::terminal, 4, 4, CB(ConsolePanel)};
    DockSpace DockSpace::FILES{3, "Files", Icons::folder_open, 4, 4, CB(FilesPanel)};
    DockSpace DockSpace::METRICS{4, "Metrics", Icons::bar_chart, 4, 4, CB(MetricsPanel)};
    const char *DockSpace::OPTIONS = "Inspector\0Hierarchy\0Console\0Files\0Metrics";


    DockSpace *DockSpace::GetOption(const int selected) {
        switch (selected) {
            case 0:
                return &INSPECTOR;
            case 1:
                return &HIERARCHY;
            case 2:
                return &CONSOLE;
            case 3:
                return &FILES;
            case 4:
                return &METRICS;
            default:
                return nullptr;
        }
    }
}
