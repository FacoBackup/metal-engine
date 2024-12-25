#ifndef VOXELIZATIONREPOSITORY_H
#define VOXELIZATIONREPOSITORY_H
#include "../../common/inspection/Inspectable.h"

namespace Metal {
    struct VoxelizationRepository final : Inspectable {
        int maxDepth = 6;
        bool randomColors = true;
        bool showRaySearchCount = true;
        bool showRayTestCount = true;

        const char *getTitle() override;

        const char *getIcon() override;

        void registerFields() override;
    };
} // Metal

#endif //VOXELIZATIONREPOSITORY_H
