#ifndef CAMERAPOSITIONPANEL_H
#define CAMERAPOSITIONPANEL_H
#include <imgui.h>
#include <glm/ext/scalar_constants.hpp>

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class CameraPositionPanel final : public AbstractPanel {
        ImVec4 RED = ImVec4(1, 0, 0, 1);
        ImVec4 GREEN = ImVec4(0, 1, 0, 1);
        ImVec4 BLUE = ImVec4(0, .5f, 1, 1);
        float TO_DEG = 180.f / glm::pi<float>();

    public:
        void onSync() override;
    };
} // Metal

#endif //CAMERAPOSITIONPANEL_H
