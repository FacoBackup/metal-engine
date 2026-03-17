#ifndef GIZMOPANEL_H
#define GIZMOPANEL_H
#include <array>
#include <imgui.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>

#include "../../abstract/AbstractPanel.h"

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
        GizmoTransformStrategy *gizmoStrategy = nullptr;

        glm::vec2 *size = nullptr;
        ImVec2 *position = nullptr;
        TransformComponent *localSelected = nullptr;
        int localChangeId{};
        bool isGizmoOver = false;

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
                {"HistoryService", &historyService}
            };
        }

        explicit GizmoPanel(ImVec2 *position, glm::vec2 *size);
        ~GizmoPanel() override;

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //GIZMOPANEL_H
