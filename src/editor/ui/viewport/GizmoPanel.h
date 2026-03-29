#ifndef GIZMOPANEL_H
#define GIZMOPANEL_H
#include <array>
#include <imgui.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>

#include "editor/ui/abstract/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;
    struct WorldRepository;
    class SelectionService;
}

namespace Metal {
    struct TransformComponent;

    class HistoryService;

    class GizmoTransformStrategy;

    class GizmoPanel final : public AbstractPanel {

        glm::vec2 *size = nullptr;
        ImVec2 *position = nullptr;
        TransformComponent *localSelected = nullptr;

        GizmoTransformStrategy *gizmoStrategy = nullptr;
        EditorRepository *editorRepository = nullptr;
        WorldRepository *worldRepository = nullptr;
        SelectionService *selectionService = nullptr;
        HistoryService *historyService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"WorldRepository", &worldRepository},
                {"SelectionService", &selectionService},
                {"HistoryService", &historyService},
                {"GizmoTransformStrategy", &gizmoStrategy}
            };
        }

        explicit GizmoPanel(ImVec2 *position, glm::vec2 *size);

        void onSync() override;
    };
} // Metal

#endif //GIZMOPANEL_H
