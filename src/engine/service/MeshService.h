#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../common/AbstractResourceService.h"
#include "../resource/MeshInstance.h"
#include "editor/enum/engine-definitions.h"
#include "../../common/IInit.h"
#include "../../common/IEventMember.h"
#include "ApplicationEventContext.h"

namespace Metal {
    struct MeshData;
    struct MeshInstance;
    struct EntityAssetData;

    class BufferService;
    class RayTracingService;
    class DirectoryService;
    struct WorldRepository;

    class MeshService final : public AbstractResourceService<MeshInstance>, public IInit {
        BufferService *bufferService = nullptr;
        RayTracingService *rayTracingService = nullptr;
        DirectoryService *directoryService = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        MeshService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"BufferService", &bufferService},
                {"RayTracingService", &rayTracingService},
                {"DirectoryService", &directoryService},
                {"WorldRepository", &worldRepository}
            };
        }

        void onInitialize() override;

        MeshInstance *create(const std::string &meshPath);

        MeshInstance *stream(const std::string &id);

        MeshData *loadMeshData(const std::string &meshPath) const;

        void disposeResource(MeshInstance *resource) override;
    };
} // Metal

#endif //MESHSERVICE_H
