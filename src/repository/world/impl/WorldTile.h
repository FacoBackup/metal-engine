#ifndef WORLDTILE_H
#define WORLDTILE_H
#include <array>
#include <string>
#include <vector>

#include "BoundingBox.h"
#include "../../../enum/engine-definitions.h"
#include "../../../service/voxel/impl/SparseVoxelOctreeBuilder.h"
#include "../../../util/serialization-definitions.h"
#define TILE_SIZE 32

namespace Metal {
    struct WorldTile final {
        std::array<std::string, 8> adjacentTiles{};
        int x;
        int z;
        std::string id;
        bool loaded = false;
        std::vector<EntityID> entities{};
        BoundingBox boundingBox{};
        int normalizedDistance = 0;

        void updateTiles(const WorldTile *adjacentWorldTile, const std::string &key) {
            bool isWest = adjacentWorldTile->z < z && adjacentWorldTile->x == x;
            bool isEast = adjacentWorldTile->z > z && adjacentWorldTile->x == x;
            bool isNorth = adjacentWorldTile->x > x && adjacentWorldTile->z == z;
            bool isSouth = adjacentWorldTile->x < x && adjacentWorldTile->z == z;

            bool isSouthEast = adjacentWorldTile->x < x && adjacentWorldTile->z > z;
            bool isSouthWest = adjacentWorldTile->x < x && adjacentWorldTile->z < z;
            bool isNorthEast = adjacentWorldTile->x > x && adjacentWorldTile->z > z;
            bool isNorthWest = adjacentWorldTile->x > x && adjacentWorldTile->z < z;

            if (isWest) adjacentTiles[0] = key;
            if (isNorth) adjacentTiles[1] = key;
            if (isEast) adjacentTiles[2] = key;
            if (isSouth) adjacentTiles[3] = key;
            if (isSouthWest) adjacentTiles[4] = key;
            if (isSouthEast) adjacentTiles[5] = key;
            if (isNorthWest) adjacentTiles[6] = key;
            if (isNorthEast) adjacentTiles[7] = key;
        }

        explicit WorldTile(int x, int z, const std::string &id)
            : x(x), z(z), id(id) {
            boundingBox.center.x = x * TILE_SIZE;
            boundingBox.center.z = z * TILE_SIZE;
            boundingBox.max = boundingBox.center + glm::vec3(TILE_SIZE / 2.0f, TILE_SIZE / 2.0f, TILE_SIZE / 2.0f);
            boundingBox.min = boundingBox.center - glm::vec3(TILE_SIZE / 2.0f, TILE_SIZE / 2.0f, TILE_SIZE / 2.0f);
        }

        explicit WorldTile(): x(0), z(0) {
        }

        void putAdjacentTile(const WorldTile *adjacentWorldTile) {
            updateTiles(adjacentWorldTile, adjacentWorldTile->id);
        }

        void removeAdjacentTile(const WorldTile *adjacentWorldTile) {
            updateTiles(adjacentWorldTile, "");
        }

        static std::string GetId(int x, int z) {
            return x + "_" + z;
        }

        SERIALIZE_TEMPLATE(
            adjacentTiles,
            x,
            z,
            id,
            loaded,
            entities,
            boundingBox,
            normalizedDistance
        )
    };
} // Metal

#endif //WORLDTILE_H
