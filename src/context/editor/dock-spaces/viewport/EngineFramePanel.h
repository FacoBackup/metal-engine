#ifndef ENGINE_FRAME_PANEL_H
#define ENGINE_FRAME_PANEL_H

#include "../../abstract/AbstractPanel.h"
#include <memory>
#include <imgui.h>

namespace Metal {
    class EngineFrame;

    class EngineFramePanel final : public AbstractPanel {
        std::unique_ptr<EngineFrame> engineFrame;

    public:
        void onInitialize() override;

        void onSync() override;

        void handleViewportPicking(const ImVec2 &imageMin, const ImVec2 &imageMax) const;
    };
} // Metal

#endif //ENGINE_FRAME_PANEL_H
