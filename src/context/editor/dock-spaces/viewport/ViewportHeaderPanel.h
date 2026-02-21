#ifndef VIEWPORTHEADERPANEL_H
#define VIEWPORTHEADERPANEL_H
#include <imgui.h>

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class ViewportHeaderPanel final : public AbstractPanel {
        static constexpr int FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                     ImGuiWindowFlags_NoCollapse;
        AbstractPanel *gizmo = nullptr;
        int shadingModelOption = 0;

    public:
        static constexpr float HEIGHT = 28;

        void onInitialize() override;

        void onSync() override;

        void cameraMode() const;

        void shadingMode();
    };
} // Metal

#endif //VIEWPORTHEADERPANEL_H
