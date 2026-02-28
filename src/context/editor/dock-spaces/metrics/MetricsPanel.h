#ifndef METRICSPANEL_H
#define METRICSPANEL_H

#include "../docks/AbstractDockPanel.h"
#include "imgui.h"

namespace Metal {
    class MetricsPanel final : public AbstractDockPanel {
    public:
        void onSync() override;
    };
}

#endif
