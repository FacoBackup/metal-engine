#ifndef VIEWPORTHEADERPANEL_H
#define VIEWPORTHEADERPANEL_H
#include <imgui.h>

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class ViewportHeaderPanel final : public AbstractPanel {
        static constexpr int FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoCollapse;
        static constexpr ImVec2 PADDING{2, 2};

        AbstractPanel *gizmo = nullptr;
        int shadingModelOption = 0;

    public:
        static constexpr float HEIGHT = 25;

        void onInitialize() override;

        void onSync() override;

        void cameraMode() const;

        void shadingMode();
    };
} // Metal

#endif //VIEWPORTHEADERPANEL_H
