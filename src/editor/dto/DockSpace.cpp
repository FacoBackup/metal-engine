#include "DockSpace.h"
#include "../../common/Icons.h"
#include "../dock-spaces/console/ConsolePanel.h"
#include "../dock-spaces/files/FilesPanel.h"
#include "../dock-spaces/metrics/MetricsPanel.h"
#include "../dock-spaces/repositories/RepositoriesPanel.h"
#include "../dock-spaces/inspector/InspectorPanel.h"
#include "../dock-spaces/viewport/ViewportPanel.h"
#include "../dock-spaces/world/WorldPanel.h"
#include "../dock-spaces/lua-editor/LuaEditorPanel.h"

#define CB(clazz) []() { return std::make_shared<clazz>(); }

namespace Metal {
    DockSpace DockSpace::VIEWPORT{-1, "Viewport", 0, 0, ImVec4{0.7f, 0.7f, 0.7f, 1}, CB(ViewportPanel)};
    DockSpace DockSpace::INSPECTOR{0, "Inspector", 4, 4, ImVec4{0.2f, 0.8f, 0.2f, 1}, CB(InspectorPanel)};
    DockSpace DockSpace::WORLD{1, "World", 4, 4, ImVec4{0.2f, 0.2f, 0.8f, 1}, CB(WorldPanel)};
    DockSpace DockSpace::CONSOLE{2, "Console", 4, 4, ImVec4{0.8f, 0.8f, 0.2f, 1}, CB(ConsolePanel)};
    DockSpace DockSpace::FILES{3, "Files", 4, 4, ImVec4{0.8f, 0.2f, 0.8f, 1}, CB(FilesPanel)};
    DockSpace DockSpace::METRICS{4, "Metrics", 4, 4, ImVec4{0.2f, 0.8f, 0.8f, 1}, CB(MetricsPanel)};
    DockSpace DockSpace::REPOSITORIES{5, "Repositories", 4, 4, ImVec4{0.8f, 0.2f, 0.2f, 1}, CB(RepositoriesPanel)};
    DockSpace DockSpace::LUA_EDITOR{6, "Lua Editor", 4, 4, ImVec4{0.2f, 0.5f, 0.8f, 1}, CB(LuaEditorPanel)};

    const std::vector<DockSpace *> DockSpace::OPTIONS_LIST = {
        &INSPECTOR,
        &WORLD,
        &CONSOLE,
        &FILES,
        &METRICS,
        &REPOSITORIES,
        &LUA_EDITOR
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
