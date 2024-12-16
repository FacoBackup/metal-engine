#ifndef GIZMOPANEL_H
#define GIZMOPANEL_H
#include <imgui.h>
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/detail/type_quat.hpp>

#include "../../common/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;
}

namespace Metal {
    struct TransformComponent;

    class GizmoPanel final : public AbstractPanel {
        glm::mat4x4 auxMat4{};
        glm::vec3 auxTranslation{};
        glm::vec3 auxScale{};
        glm::quat auxRot{};

        float *cacheMatrix = new float[16];
        float *viewMatrixCache = new float[16];
        float *projectionMatrixCache = new float[16];
        glm::vec2 *size = nullptr;
        ImVec2 *position = nullptr;
        TransformComponent *localSelected = nullptr;
        int localChangeId{};
        bool isGizmoOver = false;
        EditorRepository *editorRepository = nullptr;

    public:
        explicit GizmoPanel(ImVec2 *position, glm::vec2 *size);

        void onInitialize() override;

        void onSync() override;

        float getSnapValues() const;

        void decomposeMatrix();

        void recomposeMatrix();
    };
} // Metal

#endif //GIZMOPANEL_H
