#ifndef VOXELIZATIONREPOSITORY_H
#define VOXELIZATIONREPOSITORY_H
#include "../../common/inspection/Inspectable.h"

namespace Metal {
    struct VoxelizationRepository final : Inspectable {
        int maxDepth = 6;

        const char *getTitle() override;

        const char *getIcon() override;

        void registerFields() override;
    };
} // Metal

#endif //VOXELIZATIONREPOSITORY_H
