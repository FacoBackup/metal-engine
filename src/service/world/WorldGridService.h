#ifndef WORLDGRIDSERVICE_H
#define WORLDGRIDSERVICE_H
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct WorldTile;

    class WorldGridService final : public AbstractRuntimeComponent {
        WorldTile *prevTile = nullptr;
        bool changed = false;

    public:
        explicit WorldGridService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void addMissingTiles();

        static bool IsTileOutsideBounds(WorldTile *tile, int half, int min);

        void removeExtraTiles();

        void onSync() override;
    };
} // Metal

#endif //WORLDGRIDSERVICE_H
