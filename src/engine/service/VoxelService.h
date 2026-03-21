#ifndef METAL_ENGINE_VOXELSERVICE_H
#define METAL_ENGINE_VOXELSERVICE_H

#include "../../common/AbstractResourceService.h"
#include "../resource/SVOInstance.h"
#include "../../common/IInit.h"

namespace Metal {
    struct SVOInstance;
    class BufferService;

    class VoxelService final : public AbstractResourceService<SVOInstance>, public IInit {
        BufferService *bufferService = nullptr;
    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"BufferService", &bufferService}
            };
        }

        SVOInstance *create(const std::string &id);

        SVOInstance *stream(const std::string &id);

        void disposeResource(SVOInstance *resource) override;

        void onInitialize() override;
    };
} // Metal

#endif //METAL_ENGINE_VOXELSERVICE_H