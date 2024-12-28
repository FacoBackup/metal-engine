#ifndef VOXELDEBUGSETTINGSUBO_H
#define VOXELDEBUGSETTINGSUBO_H

namespace Metal {
    struct VoxelDebugSettingsPushConstant final {
        alignas(4) bool randomColors;
        alignas(4) bool showRaySearchCount;
        alignas(4) bool showRayTestCount;
    };
}

#endif //VOXELDEBUGSETTINGSUBO_H
