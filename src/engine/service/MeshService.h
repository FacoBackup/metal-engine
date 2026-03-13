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

    class MeshService final : public AbstractResourceService<MeshInstance> {
        BufferService &bufferService;
        RayTracingService &rayTracingService;
        const std::string &rootDirectory;

    public:
        MeshService(BufferService &bufferService, RayTracingService &rayTracingService, const std::string &rootDirectory)
            : bufferService(bufferService), rayTracingService(rayTracingService), rootDirectory(rootDirectory) {}
        MeshService() = delete;

        MeshInstance *create(const std::string &id);

        MeshInstance *stream(const std::string &id);

        MeshData *loadMeshData(const std::string &id) const;

        void disposeResource(MeshInstance *resource) override;

    };
} // Metal

#endif //MESHSERVICE_H
