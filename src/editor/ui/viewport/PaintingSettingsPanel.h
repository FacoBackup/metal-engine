#ifndef PAINTINGSETTINGSPANEL_H
#define PAINTINGSETTINGSPANEL_H
#include <imgui.h>

#include "../../common/AbstractPanel.h"

namespace Metal {
    class PaintingSettingsPanel final : public AbstractPanel {
        ImVec2 maskRes{};

    public:
        void onSync() override;
    };
} // Metal

#endif //PAINTINGSETTINGSPANEL_H
