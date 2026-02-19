#ifndef METAL_ENGINE_VOXELSERVICE_H
#define METAL_ENGINE_VOXELSERVICE_H
#include "../abstract/AbstractResourceService.h"

namespace Metal {
    struct SVOInstance;

    class VoxelService final : public AbstractResourceService {
    public:
        explicit VoxelService()
            : AbstractResourceService() {
        }

        SVOInstance *create(const std::string &id);
    };
} // Metal

#endif //METAL_ENGINE_VOXELSERVICE_H