#include "WorldGridService.h"

#include "../../context/ApplicationContext.h"
#include "../log/LogService.h"

namespace Metal {
    void WorldGridService::addMissingTiles() {
        const int numberOfTiles = context.engineRepository.numberOfTiles;
        if (const int squared = numberOfTiles * numberOfTiles;
            squared > context.worldGridRepository.getTiles().size()) {
            LOG_INFO(context, "Adding missing tiles " + std::to_string(squared) + " " + std::to_string(context.worldGridRepository.getTiles().size()));
            const int half = numberOfTiles / 2;
            for (int x = -half; x < half; x++) {
                for (int z = -half; z < half; z++) {
                    context.worldGridRepository.createIfAbsent(x, z);
                    changed = true;
                }
            }
        }
    }

    bool WorldGridService::IsTileOutsideBounds(WorldTile *tile, int half, int min) {
        return tile->z >= half || tile->z <= min || tile->x >= half || tile->x <= min;
    }

    void WorldGridService::removeExtraTiles() {
        const int numberOfTiles = context.engineRepository.numberOfTiles;
        if (const int squared = numberOfTiles * numberOfTiles;
            squared < context.worldGridRepository.getTiles().size()) {
            LOG_INFO(context, "Removing extra tiles " + std::to_string(context.worldGridRepository.getTiles().size()) + " " + std::to_string(squared));

            const int min = -numberOfTiles;
            for (auto it = context.worldGridRepository.getTiles().begin();
                 it != context.worldGridRepository.getTiles().end();) {
                auto &tile = it->second;
                if (IsTileOutsideBounds(&tile, numberOfTiles / 2, min)) {
                    LOG_INFO(context, "Removing tile " + tile.id);
                    // TODO - ADD DISPOSAL OF THINGS RATED TO THE TILE LIKE TERRAIN, FOLIAGE, MATERIALS AND VOXELS
                    it = context.worldGridRepository.getTiles().erase(it);
                    changed = true;
                } else {
                    ++it;
                }
            }
        }
    }

    void WorldGridService::onSync() {
        if (!context.worldGridRepository.updateLoadedTiles()) {
            return;
        }
        prevTile = context.worldGridRepository.getCurrentTile();
        addMissingTiles();
        removeExtraTiles();
        if (changed) {
            changed = false;
            for (auto &tile: context.worldGridRepository.getTiles()) {
                context.worldGridRepository.updateAdjacentTiles(&tile.second);
            }
        }
    }
} // Metal
