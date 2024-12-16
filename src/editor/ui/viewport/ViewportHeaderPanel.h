#ifndef VIEWPORTHEADERPANEL_H
#define VIEWPORTHEADERPANEL_H
#include <imgui.h>

#include "../../common/AbstractPanel.h"

namespace Metal {
    class ViewportHeaderPanel final : public AbstractPanel {
        static constexpr int FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoCollapse;
        static constexpr ImVec2 PADDING{4, 4};
        static constexpr float HEIGHT = 35;

        AbstractPanel *paintGizmo = nullptr;
        AbstractPanel *gizmo = nullptr;

    public:
        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //VIEWPORTHEADERPANEL_H
