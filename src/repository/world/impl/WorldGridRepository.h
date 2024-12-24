#ifndef WORLDGRID_H
#define WORLDGRID_H
#include <unordered_map>

#include "WorldTile.h"
#include "../../../common/AbstractRuntimeComponent.h"
#include "../../../enum/engine-definitions.h"

namespace Metal {
    class WorldGridRepository final : public AbstractRuntimeComponent {
        std::unordered_map<std::string, WorldTile> tiles{};
        std::array<WorldTile *, 9> loadedWorldTiles{};
        WorldTile *currentTile = nullptr;

    public:
        explicit WorldGridRepository(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        static int getTileLocation(const float v) {
            return static_cast<int>(std::floor(v / TILE_SIZE));
        }

        /**
         * Returns current tile and its adjacent ones
         * @return May contain null elements
         */
        std::array<WorldTile *, 9> &getLoadedTiles();

        WorldTile *getOrCreateTile(const glm::vec3 &point);

        WorldTile *getCurrentTile() const;

        void createIfAbsent(int x, int z);

        void addTile(const glm::vec3 &point);

        void updateAdjacentTiles(WorldTile *newTile);

        void putAdjacentTile(const std::array<int, 2> &tileLocation, WorldTile *newTile);

        void removeTile(const std::string &id);

        void moveBetweenTiles(EntityID entityId, WorldTile *previousWorldTile, WorldTile *newWorldTile) const;

        SAVE_TEMPLATE(tiles)
    };
} // Metal

#endif //WORLDGRID_H
