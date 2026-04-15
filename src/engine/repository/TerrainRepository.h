#ifndef TERRAINREPOSITORY_H
#define TERRAINREPOSITORY_H

#include "../../common/IRepository.h"
#include <string>

namespace Metal {
    struct TerrainRepository final : IRepository {
        float terrainSize = 10000.0f;
        int terrainMaxLOD = 8;
        float terrainHeight = 1000.0f;
        std::string heightMap = "";

        std::string layer1 = "";
        std::string layer2 = "";
        std::string layer3 = "";
        std::string layer4 = "";

        void clear() override {
            terrainSize = 10000.0f;
            terrainMaxLOD = 8;
            terrainHeight = 1000.0f;
            heightMap = "";
            layer1 = "";
            layer2 = "";
            layer3 = "";
            layer4 = "";
        }

        void registerFields() override;

        const char *getIcon() const override {
            return "terrain";
        }

        const char *getTitle() const override {
            return "Terrain Settings";
        }
    };
} // Metal

#endif // TERRAINREPOSITORY_H
