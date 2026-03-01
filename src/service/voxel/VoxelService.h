#ifndef METAL_ENGINE_VOXELSERVICE_H
#define METAL_ENGINE_VOXELSERVICE_H

#include "../abstract/IStreamable.h"
#include "SVOInstance.h"

namespace Metal {
    struct SVOInstance;

    class VoxelService final : public IStreamable<SVOInstance> {
    public:
        SVOInstance *create(const std::string &id) override;

        void disposeResource(SVOInstance *resource) override;
    };
} // Metal

#endif //METAL_ENGINE_VOXELSERVICE_H