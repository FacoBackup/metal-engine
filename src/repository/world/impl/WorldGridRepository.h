#ifndef WORLDGRID_H
#define WORLDGRID_H
#include <unordered_map>

#include "WorldTile.h"
#include "../../../common/AbstractRuntimeComponent.h"
#include "../../../common/inspection/Inspectable.h"
#include "../../../enum/engine-definitions.h"
#include "../../../util/Serializable.h"

namespace Metal {
    class WorldGridRepository final : public AbstractRuntimeComponent, public Serializable {
        std::unordered_map<std::string, WorldTile> tiles{};
        std::array<WorldTile *, 9> loadedWorldTiles{};
        WorldTile *currentTile = nullptr;
        unsigned int prevSize = 0;

    public:
        bool hasMainTileChanged = false;

        bool updateLoadedTiles();

        explicit WorldGridRepository()
            : AbstractRuntimeComponent() {
        }

        static int getTileLocation(const float v) {
            return static_cast<int>(std::floor(v / TILE_SIZE) + 0.5);
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

        nlohmann::json toJson() const override {
            nlohmann::json j;
            nlohmann::json t;
            for (auto const& [key, val] : tiles) {
                t[key] = val.toJson();
            }
            j["tiles"] = t;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            tiles.clear();
            if (j.contains("tiles")) {
                for (auto const& [key, val] : j.at("tiles").items()) {
                    WorldTile tile;
                    tile.fromJson(val);
                    tiles.emplace(key, tile);
                }
            }
        }
    };
} // Metal

#endif //WORLDGRID_H
