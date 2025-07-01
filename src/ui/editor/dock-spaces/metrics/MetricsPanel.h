#ifndef METRICSPANEL_H
#define METRICSPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class MetricsPanel final : public AbstractPanel {
        public:
        void onSync() override;
    };
}

#endif //METRICSPANEL_H
