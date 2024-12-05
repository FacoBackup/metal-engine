#include "DockSpace.h"
#include "../../../common/Icons.h"
#include "../../ui/console/ConsolePanel.h"
#include "../../ui/files/FilesPanel.h"
#include "../../ui/inspector/InspectorPanel.h"
#include "../../ui/metrics/MetricsPanel.h"
#include "../../ui/viewport/ViewportPanel.h"
#include "../../ui/hierarchy/HierarchyPanel.h"

#define CB(clazz) []() { return new clazz; }

namespace Metal {
    DockSpace DockSpace::VIEWPORT{0, "Viewport", Icons::ipublic, 0, 0,CB(ViewportPanel)};
    DockSpace DockSpace::INSPECTOR{1, "Inspector", Icons::search, 4, 4,CB(InspectorPanel)};
    DockSpace DockSpace::HIERARCHY{2, "Hierarchy", Icons::account_tree, 4, 4, CB(HierarchyPanel)};
    DockSpace DockSpace::CONSOLE{3, "Console", Icons::terminal, 4, 4, CB(ConsolePanel)};
    DockSpace DockSpace::FILES{4, "Files", Icons::folder_open, 4, 4, CB(FilesPanel)};
    DockSpace DockSpace::METRICS{5, "Metrics", Icons::bar_chart, 4, 4, CB(MetricsPanel)};
    const char *DockSpace::OPTIONS = "Viewport\0Inspector\0Hierarchy\0Console\0Files\0Metrics";


    DockSpace *DockSpace::GetOption(const int selected) {
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
            default:
                return nullptr;
        }
    }
}
