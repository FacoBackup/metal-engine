#ifndef CAMERA_GIZMO_PANEL_H
#define CAMERA_GIZMO_PANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include <glm/mat4x4.hpp>

namespace Metal {
    struct CameraRepository;

    class CameraGizmoPanel final : public AbstractPanel {
        CameraRepository *cameraRepository = nullptr;
        glm::mat4 tempView{1.0f};
        bool isManipulating = false;
        static constexpr float GIZMO_SIZE = 100.0f;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"CameraRepository", &cameraRepository}
            };
        }

        void onSync() override;
    };
} // Metal

#endif // CAMERA_GIZMO_PANEL_H
