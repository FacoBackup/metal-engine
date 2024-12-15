#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class InspectorPanel final : public AbstractDockPanel {
    public:
        void onInitialize() override;

        void onSync() override;
    };
}

#endif
