#ifndef HIERARCHYHEADERPANEL_H
#define HIERARCHYHEADERPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class WorldHeaderPanel final : public AbstractPanel {
    public:
        void onSync() override;

        char search[512];
        int selectedComponentType = -1;
    };
} // Metal

#endif //HIERARCHYHEADERPANEL_H
