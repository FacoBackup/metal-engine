#ifndef VOXELDEBUGSETTINGSUBO_H
#define VOXELDEBUGSETTINGSUBO_H

namespace Metal {
    struct VoxelDebugSettingsPushConstant final {
        alignas(4) int voxelDebugFlag;
        alignas(4) bool showRaySearchCount;
        alignas(4) bool showRayTestCount;
    };
}

#endif //VOXELDEBUGSETTINGSUBO_H
