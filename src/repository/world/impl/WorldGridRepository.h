#ifndef WORLDGRID_H
#define WORLDGRID_H
#include <unordered_map>

#include "WorldTile.h"
#include "../../../common/AbstractRuntimeComponent.h"
#include "../../../common/inspection/Inspectable.h"
#include "../../../enum/engine-definitions.h"

namespace Metal {
    class WorldGridRepository final : public AbstractRuntimeComponent, public Inspectable {
        int numberOfTiles = 10;
        std::unordered_map<std::string, WorldTile> tiles{};
        std::array<WorldTile *, 9> loadedWorldTiles{};
        WorldTile *currentTile = nullptr;
        unsigned int prevSize = 0;

    public:
        [[nodiscard]] int getNumberOfTiles() const {
            return numberOfTiles;
        }

        bool updateLoadedTiles();

        explicit WorldGridRepository(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        static int getTileLocation(const float v) {
            return static_cast<int>(std::floor((v + TILE_SIZE / 2.f) / TILE_SIZE));
        }

        /**
         * Returns current tile and its adjacent ones
         * @return May contain null elements
         */
        std::array<WorldTile *, 9> &getLoadedTiles();

        WorldTile *getOrCreateTile(const glm::vec3 &point);

        WorldTile *getCurrentTile() const;

        WorldTile *getTile(const glm::vec3 &point);

        void createIfAbsent(int x, int z);

        void addTile(const glm::vec3 &point);

        void updateAdjacentTiles(WorldTile *newTile);

        void putAdjacentTile(const std::array<int, 2> &tileLocation, WorldTile *newTile);

        void removeTile(const std::string &id);

        void moveBetweenTiles(EntityID entityId, WorldTile *previousWorldTile, WorldTile *newWorldTile) const;

        std::unordered_map<std::string, WorldTile> &getTiles() {
            return tiles;
        }

        const char *getIcon() override;

        const char *getTitle() override;

        void registerFields() override;

        SAVE_TEMPLATE(tiles, numberOfTiles)
    };
} // Metal

#endif //WORLDGRID_H
