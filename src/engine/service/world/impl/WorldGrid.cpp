#include "WorldGrid.h"

namespace Metal {
    std::array<WorldTile *, 9> &WorldGrid::getLoadedTiles(const glm::vec3 &point) {
        auto *center = getOrCreateTile(point);
        loadedWorldTiles[0] = center;
        for (int i = 0; i < 8; i++) {
            loadedWorldTiles[i + 1] = tiles.at(center->adjacentTiles[i]);
        }
        return loadedWorldTiles;
    }

    WorldTile *WorldGrid::getOrCreateTile(const glm::vec3 &point) {
        const int tileX = getTileLocation(point.x);
        const int tileZ = getTileLocation(point.z);
        const std::string id = WorldTile::GetId(tileX, tileZ);
        if (!tiles.contains(id)) {
            addTile(point);
        }
        return tiles.at(id);
    }

    void WorldGrid::createIfAbsent(const int x, const int z) {
        if (std::string id = WorldTile::GetId(x, z); !tiles.contains(id)) {
            auto newTile = new WorldTile(x, z, id);
            tiles.insert({id, newTile});
        }
    }

    void WorldGrid::addTile(const glm::vec3 &point) {
        int x = getTileLocation(point.x);
        int z = getTileLocation(point.z);
        std::string id = WorldTile::GetId(x, z);
        if (tiles.contains(id)) {
            return;
        }
        auto newTile = new WorldTile(x, z, id);
        tiles.insert({id, newTile});

        updateAdjacentTiles(newTile);
    }

    void WorldGrid::updateAdjacentTiles(WorldTile *newTile) const {
        const int x = newTile->x;
        const int z = newTile->z;
        const std::array westTile{x, z - 1};
        const std::array eastTile{x, z + 1};
        const std::array northTile{x + 1, z};
        const std::array southTile{x - 1, z};
        const std::array northEastTile{x + 1, z + 1};
        const std::array northWestTile{x + 1, z - 1};
        const std::array southEastTile{x - 1, z + 1};
        const std::array southWestTile{x - 1, z - 1};

        putAdjacentTile(westTile, newTile);
        putAdjacentTile(eastTile, newTile);
        putAdjacentTile(northTile, newTile);
        putAdjacentTile(southTile, newTile);
        putAdjacentTile(northEastTile, newTile);
        putAdjacentTile(northWestTile, newTile);
        putAdjacentTile(southEastTile, newTile);
        putAdjacentTile(southWestTile, newTile);
    }

    void WorldGrid::putAdjacentTile(const std::array<int, 2> &tileLocation, WorldTile *newTile) const {
        if (const std::string tileId = WorldTile::GetId(tileLocation[0], tileLocation[1]); tiles.contains(tileId)) {
            auto *tile = tiles.at(tileId);
            tile->putAdjacentTile(newTile);
            newTile->putAdjacentTile(tile);
        }
    }

    void WorldGrid::removeTile(const std::string &id) {
        const auto *tileToRemove = tiles.at(id);
        tiles.erase(id);
        for (auto &tile: tiles) {
            tile.second->removeAdjacentTile(tileToRemove);
        }
    }

    void WorldGrid::MoveBetweenTiles(const EntityID entityId, WorldTile *previousWorldTile, WorldTile *newWorldTile) {
        if (newWorldTile != previousWorldTile) {
            bool inserted = false;
            bool removed = false;
            for (unsigned int i = 0; i < MAX_ENTITIES; i++) {
                if (!removed && previousWorldTile->entities[i] == entityId) {
                    previousWorldTile->entities[i] = EMPTY_ENTITY;
                    removed = true;
                }

                if (!inserted && newWorldTile->entities[i] == EMPTY_ENTITY) {
                    inserted = true;
                    newWorldTile->entities[i] = entityId;
                }

                if (inserted && removed) {
                    break;
                }
            }
        }
    }
} // Metal
