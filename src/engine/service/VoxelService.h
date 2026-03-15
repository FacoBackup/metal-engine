#ifndef METAL_ENGINE_VOXELSERVICE_H
#define METAL_ENGINE_VOXELSERVICE_H

#include "../../common/AbstractResourceService.h"
#include "../resource/SVOInstance.h"

namespace Metal {
    struct SVOInstance;

    class VoxelService final : public AbstractResourceService<SVOInstance> {
    public:
        SVOInstance *create(const std::string &id);

        SVOInstance *stream(const std::string &id);

        void disposeResource(SVOInstance *resource) override;
    };
} // Metal

#endif //METAL_ENGINE_VOXELSERVICE_H