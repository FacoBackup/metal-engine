#ifndef HIERARCHYHEADERPANEL_H
#define HIERARCHYHEADERPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct WorldRepository;

    class WorldHeaderPanel final : public AbstractPanel {
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", worldRepository}
            };
        }

        void onSync() override;

        char search[512];
        int selectedComponentType = -1;
    };
} // Metal

#endif //HIERARCHYHEADERPANEL_H
