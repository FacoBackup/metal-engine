#ifndef CONSOLEPANEL_H
#define CONSOLEPANEL_H
#include "../docks/AbstractDockPanel.h"
#include "../../service/LogService.h"
#include "imgui.h"

namespace Metal {
    class ConsolePanel final : public AbstractDockPanel {
    public:
        void onSync() override;

        explicit ConsolePanel();
    private:
        ImGuiTextFilter filter;
        bool autoScroll = true;
        bool levelFilter[6];
    };
}

#endif
