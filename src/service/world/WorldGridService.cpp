#include "WorldGridService.h"

#include "../../context/ApplicationContext.h"
#include "../log/LogService.h"

namespace Metal {
    void WorldGridService::addMissingTiles() {
        const int numberOfTiles = ApplicationContext::Get().engineRepository.numberOfTiles;
        if (const int squared = numberOfTiles * numberOfTiles;
            squared > ApplicationContext::Get().worldGridRepository.getTiles().size()) {
            LOG_INFO("Adding missing tiles " + std::to_string(squared) + " " + std::to_string(ApplicationContext::Get().worldGridRepository.getTiles().size()));
            const int half = numberOfTiles / 2;
            for (int x = -half; x < half; x++) {
                for (int z = -half; z < half; z++) {
                    ApplicationContext::Get().worldGridRepository.createIfAbsent(x, z);
                    changed = true;
                }
            }
        }
    }

    bool WorldGridService::IsTileOutsideBounds(WorldTile *tile, int half, int min) {
        return tile->z >= half || tile->z <= min || tile->x >= half || tile->x <= min;
    }

    void WorldGridService::removeExtraTiles() {
        const int numberOfTiles = ApplicationContext::Get().engineRepository.numberOfTiles;
        if (const int squared = numberOfTiles * numberOfTiles;
            squared < ApplicationContext::Get().worldGridRepository.getTiles().size()) {
            LOG_INFO("Removing extra tiles " + std::to_string(ApplicationContext::Get().worldGridRepository.getTiles().size()) + " " + std::to_string(squared));

            const int min = -numberOfTiles;
            for (auto it = ApplicationContext::Get().worldGridRepository.getTiles().begin();
                 it != ApplicationContext::Get().worldGridRepository.getTiles().end();) {
                auto &tile = it->second;
                if (IsTileOutsideBounds(&tile, numberOfTiles / 2, min)) {
                    LOG_INFO("Removing tile " + tile.id);
                    // TODO - ADD DISPOSAL OF THINGS RATED TO THE TILE LIKE TERRAIN, FOLIAGE, MATERIALS AND VOXELS
                    it = ApplicationContext::Get().worldGridRepository.getTiles().erase(it);
                    changed = true;
                } else {
                    ++it;
                }
            }
        }
    }

    void WorldGridService::onSync() {
        if (!ApplicationContext::Get().worldGridRepository.updateLoadedTiles()) {
            return;
        }
        prevTile = ApplicationContext::Get().worldGridRepository.getCurrentTile();
        addMissingTiles();
        removeExtraTiles();
        if (changed) {
            changed = false;
            for (auto &tile: ApplicationContext::Get().worldGridRepository.getTiles()) {
                ApplicationContext::Get().worldGridRepository.updateAdjacentTiles(&tile.second);
            }
        }
    }
} // Metal
