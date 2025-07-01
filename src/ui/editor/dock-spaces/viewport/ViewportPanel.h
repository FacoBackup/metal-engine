#ifndef METAL_ENGINE_VIEWPORTPANEL_H
#define METAL_ENGINE_VIEWPORTPANEL_H

#include <imgui.h>
#include <glm/vec2.hpp>

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class ViewportPanel final : public AbstractPanel {
        bool isFirstMovement = false;
        AbstractPanel *headerPanel = nullptr;
        AbstractPanel *gizmoPanel = nullptr;
        AbstractPanel *cameraPanel = nullptr;
        ImVec2 position{};
        glm::vec2 size{};

    public:
        void onInitialize() override;

        void onSync() override;

        void updateCamera();

        void updateInputs();
    };
}

#endif
