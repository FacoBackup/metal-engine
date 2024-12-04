#include "DockSpace.h"
#include "../../../common/Icons.h"
#include "../../ui/inspector/InspectorPanel.h"
#include "../../ui/viewport/ViewportPanel.h"

namespace Metal {
    const DockSpace DockSpace::VIEWPORT{0, "Viewport", Icons::ipublic, 0, 0,
                                        []() { return std::make_unique<ViewportPanel>(); }};
    const DockSpace DockSpace::INSPECTOR{1, "Inspector", Icons::search, 4, 4,
                                         []() { return std::make_unique<InspectorPanel>(); }};
    const DockSpace DockSpace::HIERARCHY{2, "Hierarchy", Icons::account_tree, 4, 4,
                                         []() { return new InspectorPanel; }};
    const DockSpace DockSpace::CONSOLE{3, "Console", Icons::terminal, 4, 4, []() { return new InspectorPanel; }};
    const DockSpace DockSpace::FILES{4, "Files", Icons::folder_open, 4, 4, []() { return new InspectorPanel; }};
    const DockSpace DockSpace::RESOURCES{5, "Resources", Icons::data_array, 4, 4, []() { return new InspectorPanel; }};
    const DockSpace DockSpace::METRICS{6, "Metrics", Icons::bar_chart, 4, 4, []() { return new InspectorPanel; }};
}