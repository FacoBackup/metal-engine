#include "DockSpace.h"
#include "../../common/Icons.h"
#include "../dock-spaces/console/ConsolePanel.h"
#include "../dock-spaces/files/FilesPanel.h"
#include "../dock-spaces/metrics/MetricsPanel.h"
#include "../dock-spaces/repositories/RepositoriesPanel.h"
#include "../dock-spaces/inspector/InspectorPanel.h"
#include "../dock-spaces/viewport/ViewportPanel.h"
#include "../dock-spaces/world/WorldPanel.h"

#define CB(clazz) []() { return std::make_shared<clazz>(); }

namespace Metal {
    DockSpace DockSpace::VIEWPORT{-1, "Viewport", Icons::i_public, 0, 0,CB(ViewportPanel)};
    DockSpace DockSpace::INSPECTOR{0, "Inspector", Icons::search, 4, 4,CB(InspectorPanel)};
    DockSpace DockSpace::WORLD{1, "World", Icons::account_tree, 4, 4, CB(WorldPanel)};
    DockSpace DockSpace::CONSOLE{2, "Console", Icons::terminal, 4, 4, CB(ConsolePanel)};
    DockSpace DockSpace::FILES{3, "Files", Icons::folder_open, 4, 4, CB(FilesPanel)};
    DockSpace DockSpace::METRICS{4, "Metrics", Icons::analytics, 4, 4, CB(MetricsPanel)};
    DockSpace DockSpace::REPOSITORIES{5, "Repositories", Icons::search, 4, 4, CB(RepositoriesPanel)};

    const std::vector<DockSpace *> DockSpace::OPTIONS_LIST = {
            &INSPECTOR,
            &WORLD,
            &CONSOLE,
            &FILES,
            &METRICS,
            &REPOSITORIES
    };

    DockSpace *DockSpace::GetOption(const int selected) {
        for (auto *option: OPTIONS_LIST) {
            if (option->index == selected) {
                return option;
            }
        }
        return nullptr;
    }
}
