#ifndef METAL_ENGINE_VOXELSERVICE_H
#define METAL_ENGINE_VOXELSERVICE_H

#include "../../common/AbstractResourceService.h"
#include "../resource/SVOInstance.h"
#include "../../common/IInit.h"
#include "common/ILoader.h"

namespace Metal {
    struct SVOInstance;
    class BufferService;

    class VoxelService final : public AbstractResourceService<SVOInstance>, public IInit, public ILoader {
        BufferService *bufferService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"BufferService", &bufferService}
            };
        }

        bool isCompatible(const std::string &absolutePath) override;

        void load(const std::string &absolutePath) override;

        void disposeResource(SVOInstance *resource) override;

        void onInitialize() override;
    };
} // Metal

#endif //METAL_ENGINE_VOXELSERVICE_H
