#ifndef TERRAINSERVICE_H
#define TERRAINSERVICE_H

#include "../../common/IService.h"
#include "../../common/IInit.h"
#include "../../common/IEventMember.h"
#include <string>
#include <vector>

namespace Metal {
    struct TerrainRepository;
    class DirectoryService;

    class TerrainService final : public IService, public IInit, public IEventMember {
        TerrainRepository *terrainRepository = nullptr;
        DirectoryService *directoryService = nullptr;

    public:
        TerrainService() = default;

        std::vector<Dependency> getDependencies() override;

        void onInitialize() override;

        void generateLODMeshes();

        std::string getLODMeshPath(int lod) const;
    };
} // Metal

#endif //TERRAINSERVICE_H
