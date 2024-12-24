#include "WorldGridRepository.h"

#include "../../../context/ApplicationContext.h"

namespace Metal {
    std::array<WorldTile *, 9> &WorldGridRepository::getLoadedTiles() {
        if (auto *center = getOrCreateTile(context.worldRepository.camera.position);
            currentTile != center) {
            currentTile = center;
            loadedWorldTiles[0] = center;
            for (int i = 0; i < 8; i++) {
                loadedWorldTiles[i + 1] = tiles.at(center->adjacentTiles[i]);
            }
        }
        return loadedWorldTiles;
    }

    WorldTile *WorldGridRepository::getOrCreateTile(const glm::vec3 &point) {
        const int tileX = getTileLocation(point.x);
        const int tileZ = getTileLocation(point.z);
        const std::string id = WorldTile::GetId(tileX, tileZ);
        if (!tiles.contains(id)) {
            addTile(point);
        }
        return tiles.at(id);
    }

    WorldTile *WorldGridRepository::getCurrentTile() const {
        return currentTile;
    }

    void WorldGridRepository::createIfAbsent(const int x, const int z) {
        if (std::string id = WorldTile::GetId(x, z); !tiles.contains(id)) {
            auto newTile = new WorldTile(x, z, id);
            tiles.insert({id, newTile});
        }
    }

    void WorldGridRepository::addTile(const glm::vec3 &point) {
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

    void WorldGridRepository::updateAdjacentTiles(WorldTile *newTile) const {
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

    void WorldGridRepository::putAdjacentTile(const std::array<int, 2> &tileLocation, WorldTile *newTile) const {
        if (const std::string tileId = WorldTile::GetId(tileLocation[0], tileLocation[1]); tiles.contains(tileId)) {
            auto *tile = tiles.at(tileId);
            tile->putAdjacentTile(newTile);
            newTile->putAdjacentTile(tile);
        }
    }

    void WorldGridRepository::removeTile(const std::string &id) {
        const auto *tileToRemove = tiles.at(id);
        tiles.erase(id);
        for (auto &tile: tiles) {
            tile.second->removeAdjacentTile(tileToRemove);
        }
    }

    void WorldGridRepository::moveBetweenTiles(const EntityID entityId, WorldTile *previousWorldTile,
                                               WorldTile *newWorldTile) const {
        auto *entity = context.worldRepository.getEntity(entityId);
        entity->registerChange();
        entity->freezeVersion();
        if (entity->onTile != nullptr) {
            previousWorldTile->entities.erase(
                std::ranges::remove(previousWorldTile->entities, entityId).begin(),
                previousWorldTile->entities.end());
            newWorldTile->entities.push_back(entityId);
        } else {
            newWorldTile->entities.push_back(entityId);
        }
        entity->onTile = newWorldTile->id.c_str();
    }
} // Metal
