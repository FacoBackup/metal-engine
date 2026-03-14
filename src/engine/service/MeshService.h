#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../common/AbstractResourceService.h"
#include "../resource/MeshInstance.h"
#include "../../editor/enum/engine-definitions.h"

namespace Metal {
    struct MeshData;
    struct MeshInstance;
    struct EntityAssetData;

    class BufferService;
    class RayTracingService;
    class DirectoryService;

    class MeshService final : public AbstractResourceService<MeshInstance> {
        BufferService *bufferService = nullptr;
        RayTracingService *rayTracingService = nullptr;
        DirectoryService *directoryService = nullptr;

    public:
        MeshService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"BufferService", bufferService},
                {"RayTracingService", rayTracingService},
                {"DirectoryService", directoryService}
            };
        }

        MeshInstance *create(const std::string &id);

        MeshInstance *stream(const std::string &id);

        MeshData *loadMeshData(const std::string &id) const;

        void disposeResource(MeshInstance *resource) override;
    };
} // Metal

#endif //MESHSERVICE_H
