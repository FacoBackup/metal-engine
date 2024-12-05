#ifndef HIERARCHYPANEL_H
#define HIERARCHYPANEL_H
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class HierarchyPanel final : public AbstractDockPanel {
    public:
        void onSync() override;
    };
} // Metal

#endif //HIERARCHYPANEL_H
