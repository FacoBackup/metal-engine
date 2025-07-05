#include "Views.h"
#include "../../common/enum/Icons.h"
#include "../editor/dock-spaces/console/ConsolePanel.h"
#include "../editor/dock-spaces/files/FilesPanel.h"
#include "../editor/dock-spaces/inspector/InspectorPanel.h"
#include "../editor/dock-spaces/metrics/MetricsPanel.h"
#include "../editor/dock-spaces/hierarchy/HierarchyPanel.h"

#define CB(clazz) []() { return new clazz; }

namespace Metal {
    const std::string Views::INSPECTOR = "Inspector";
    const std::string Views::HIERARCHY = "Hierarchy";
    const std::string Views::CONSOLE = "Console";
    const std::string Views::FILES = "Files";
    const std::string Views::METRICS = "Metrics";

    std::array<Views, 5> Views::OPTIONS = {
        Views{INSPECTOR, Icons::search, CB(InspectorPanel)},
        Views{HIERARCHY, Icons::account_tree, CB(HierarchyPanel)},
        Views{CONSOLE, Icons::terminal, CB(ConsolePanel)},
        Views{FILES, Icons::folder_open, CB(FilesPanel)},
        Views{METRICS, Icons::bar_chart, CB(MetricsPanel)}
    };

    size_t Views::FindIndexByName(const std::string &name) {
        for (size_t i = 0; i < OPTIONS.size(); ++i) {
            if (OPTIONS[i].name == name) {
                return i;
            }
        }
        return -1;
    }
}
