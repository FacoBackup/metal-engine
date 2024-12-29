#include "WorldGridService.h"

#include "../../context/ApplicationContext.h"

namespace Metal {
    void WorldGridService::addMissingTiles() {
        const int numberOfTiles = context.engineRepository.numberOfTiles;
        if (const int squared = numberOfTiles * numberOfTiles;
            squared > context.worldGridRepository.getTiles().size()) {
            std::cout << "Adding missing tiles " << squared << " " << context.worldGridRepository.getTiles().size() <<
                    std::endl;
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
            std::cout << "Removing extra tiles " << context.worldGridRepository.getTiles().size() << " " << squared <<
                    std::endl;

            const int min = -numberOfTiles;
            for (auto it = context.worldGridRepository.getTiles().begin();
                 it != context.worldGridRepository.getTiles().end();) {
                auto &tile = it->second;
                if (IsTileOutsideBounds(&tile, numberOfTiles / 2, min)) {
                    std::cout << "Removing tile " << tile.id << std::endl;
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
