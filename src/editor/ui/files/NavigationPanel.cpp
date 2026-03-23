#include "NavigationPanel.h"
#include <imgui.h>

namespace Metal {
    void NavigationPanel::onInitialize() {
        bookmarksPanel = initializePanel<BookmarksPanel>(false, filesContext);
        systemDirPanel = initializePanel<SystemDirPanel>(false, filesContext);
    }

    void NavigationPanel::onSync() {
        if (ImGui::BeginChild((id + "navigation_view").c_str(), ImVec2(0, 0))) {
            bookmarksPanel->onSync();
            ImGui::Spacing();
            systemDirPanel->onSync();
        }
        ImGui::EndChild();
    }
}
