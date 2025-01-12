#ifndef HIERARCHYHEADERPANEL_H
#define HIERARCHYHEADERPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class HierarchyHeaderPanel final : public AbstractPanel {
    public:
        void onSync() override;

        char search[512];
    };
} // Metal

#endif //HIERARCHYHEADERPANEL_H
