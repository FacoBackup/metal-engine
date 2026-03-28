#include "DockSpace.h"
#include "common/Icons.h"
#include "../ui/console/ConsolePanel.h"
#include "../ui/files/FilesPanel.h"
#include "../ui/metrics/MetricsPanel.h"
#include "../ui/repositories/RepositoriesPanel.h"
#include "../ui/inspector/InspectorPanel.h"
#include "../ui/viewport/ViewportPanel.h"
#include "../ui/world/WorldPanel.h"
#include "../ui/lua-editor/LuaEditorPanel.h"
#include "../ui/mcp/AIAssistantPanel.h"

#define CB(clazz) []() { return std::make_shared<clazz>(); }

namespace Metal {
    DockSpace DockSpace::VIEWPORT{-1, "Viewport", 0, 0, ImVec4{0.7f, 0.7f, 0.7f, 1}, CB(ViewportPanel)};
    DockSpace DockSpace::INSPECTOR{0, "Inspector", 4, 4, ImVec4{0.2f, 0.8f, 0.2f, 1}, CB(InspectorPanel)};
    DockSpace DockSpace::WORLD{1, "World", 4, 4, ImVec4{0.2f, 0.2f, 0.8f, 1}, CB(WorldPanel)};
    DockSpace DockSpace::CONSOLE{2, "Console", 4, 4, ImVec4{0.8f, 0.8f, 0.2f, 1}, CB(ConsolePanel)};
    DockSpace DockSpace::FILES{3, "Assets", 4, 4, ImVec4{0.8f, 0.2f, 0.8f, 1}, CB(FilesPanel)};
    DockSpace DockSpace::METRICS{4, "Metrics", 4, 4, ImVec4{0.2f, 0.8f, 0.8f, 1}, CB(MetricsPanel)};
    DockSpace DockSpace::REPOSITORIES{5, "Repositories", 4, 4, ImVec4{0.8f, 0.2f, 0.2f, 1}, CB(RepositoriesPanel)};
    DockSpace DockSpace::LUA_EDITOR{6, "Lua Editor", 4, 4, ImVec4{0.2f, 0.5f, 0.8f, 1}, CB(LuaEditorPanel)};
    DockSpace DockSpace::MCP{7, "AI Assistant", 4, 4, ImVec4{0.8f, 0.5f, 0.2f, 1}, CB(AIAssistantPanel)};

    const std::vector<DockSpace *> DockSpace::OPTIONS_LIST = {
        &INSPECTOR,
        &WORLD,
        &CONSOLE,
        &FILES,
        &METRICS,
        &REPOSITORIES,
        &LUA_EDITOR,
        &MCP
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
