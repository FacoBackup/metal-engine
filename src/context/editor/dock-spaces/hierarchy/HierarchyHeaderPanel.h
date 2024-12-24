#ifndef HIERARCHYHEADERPANEL_H
#define HIERARCHYHEADERPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class HierarchyHeaderPanel final : public AbstractPanel {
        char *&search;

    public:
        explicit HierarchyHeaderPanel(char * &search);

        void onSync() override;
    };
} // Metal

#endif //HIERARCHYHEADERPANEL_H
