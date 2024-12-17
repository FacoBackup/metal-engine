#ifndef WORLDGRID_H
#define WORLDGRID_H
#include <../../../../../cmake-build-debug/vcpkg_installed/arm64-osx/include/nlohmann/detail/value_t.hpp>

#include "../../../engine-definitions.h"
#include "WorldTile.h"

namespace Metal {
    class WorldGrid final {
        std::unordered_map<std::string, WorldTile *> tiles{};
        std::array<WorldTile *, 9> loadedWorldTiles{};

    public:
        static int getTileLocation(const float v) {
            return static_cast<int>(std::floor(v / TILE_SIZE));
        }

        /**
         * Returns current tile and its adjacent ones
         * @param point Approximate location of the current tile
         * @return May contain null elements
         */
        std::array<WorldTile *, 9> &getLoadedTiles(const glm::vec3 &point);

        WorldTile *getOrCreateTile(const glm::vec3 &point);

        void createIfAbsent(int x, int z);

        void addTile(const glm::vec3 &point);

        void updateAdjacentTiles(WorldTile *newTile) const;

        void putAdjacentTile(const std::array<int, 2> &tileLocation, WorldTile *newTile) const;

        std::unordered_map<std::string, WorldTile *> &getTiles() {
            return tiles;
        }

        void removeTile(const std::string &id);

        static void MoveBetweenTiles(const EntityID entityId, WorldTile *previousWorldTile, WorldTile *newWorldTile);
    };
} // Metal

#endif //WORLDGRID_H
