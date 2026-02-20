#include "WorldGridService.h"

#include "../../context/ApplicationContext.h"
#include "../log/LogService.h"

namespace Metal {
    void WorldGridService::addMissingTiles() {
        const int numberOfTiles = CTX.engineRepository.numberOfTiles;
        if (const int squared = numberOfTiles * numberOfTiles;
            squared > CTX.worldGridRepository.getTiles().size()) {
            LOG_INFO("Adding missing tiles " + std::to_string(squared) + " " + std::to_string(CTX.worldGridRepository.getTiles().size()));
            const int half = numberOfTiles / 2;
            for (int x = -half; x < half; x++) {
                for (int z = -half; z < half; z++) {
                    CTX.worldGridRepository.createIfAbsent(x, z);
                    changed = true;
                }
            }
        }
    }

    bool WorldGridService::IsTileOutsideBounds(WorldTile *tile, int half, int min) {
        return tile->z >= half || tile->z <= min || tile->x >= half || tile->x <= min;
    }

    void WorldGridService::removeExtraTiles() {
        const int numberOfTiles = CTX.engineRepository.numberOfTiles;
        if (const int squared = numberOfTiles * numberOfTiles;
            squared < CTX.worldGridRepository.getTiles().size()) {
            LOG_INFO("Removing extra tiles " + std::to_string(CTX.worldGridRepository.getTiles().size()) + " " + std::to_string(squared));

            const int min = -numberOfTiles;
            for (auto it = CTX.worldGridRepository.getTiles().begin();
                 it != CTX.worldGridRepository.getTiles().end();) {
                auto &tile = it->second;
                if (IsTileOutsideBounds(&tile, numberOfTiles / 2, min)) {
                    LOG_INFO("Removing tile " + tile.id);
                    // TODO - ADD DISPOSAL OF THINGS RATED TO THE TILE LIKE TERRAIN, FOLIAGE, MATERIALS AND VOXELS
                    it = CTX.worldGridRepository.getTiles().erase(it);
                    changed = true;
                } else {
                    ++it;
                }
            }
        }
    }

    void WorldGridService::onSync() {
        if (!CTX.worldGridRepository.updateLoadedTiles()) {
            return;
        }
        prevTile = CTX.worldGridRepository.getCurrentTile();
        addMissingTiles();
        removeExtraTiles();
        if (changed) {
            changed = false;
            for (auto &tile: CTX.worldGridRepository.getTiles()) {
                CTX.worldGridRepository.updateAdjacentTiles(&tile.second);
            }
        }
    }
} // Metal
