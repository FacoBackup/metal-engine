#include "DockSpace.h"
#include "../../../common/Icons.h"
#include "../../ui/inspector/InspectorPanel.h"
#include "../../ui/viewport/ViewportPanel.h"

#define CB(clazz) []() { return std::make_unique<clazz>(); }

namespace Metal {
    DockSpace DockSpace::VIEWPORT{0, "Viewport", Icons::ipublic, 0, 0,CB(ViewportPanel)};
    DockSpace DockSpace::INSPECTOR{1, "Inspector", Icons::search, 4, 4,CB(InspectorPanel)};
    DockSpace DockSpace::HIERARCHY{2, "Hierarchy", Icons::account_tree, 4, 4, CB(InspectorPanel)};
    DockSpace DockSpace::CONSOLE{3, "Console", Icons::terminal, 4, 4, CB(InspectorPanel)};
    DockSpace DockSpace::FILES{4, "Files", Icons::folder_open, 4, 4, CB(InspectorPanel)};
    DockSpace DockSpace::RESOURCES{5, "Resources", Icons::data_array, 4, 4, CB(InspectorPanel)};
    DockSpace DockSpace::METRICS{6, "Metrics", Icons::bar_chart, 4, 4, CB(InspectorPanel)};
    const char *DockSpace::OPTIONS = "Viewport0Inspector0Hierarchy0Console0Files0Resources0Metrics";


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
                return &RESOURCES;
            case 6:
                return &METRICS;
            default:
                return nullptr;
        }
    }
}
