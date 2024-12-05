#ifndef METRICSPANEL_H
#define METRICSPANEL_H
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class MetricsPanel final : public AbstractDockPanel {
        public:
        void onSync() override;
    };
}

#endif //METRICSPANEL_H
