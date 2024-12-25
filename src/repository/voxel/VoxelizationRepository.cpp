#include "VoxelizationRepository.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    const char *VoxelizationRepository::getTitle() {
        return "Voxelization";
    }

    const char *VoxelizationRepository::getIcon() {
        return Icons::apps.c_str();
    }

    void VoxelizationRepository::registerFields() {
        registerInt(maxDepth, "", "Max depth", 2, 15);
        registerBool(randomColors, "Debug", "Random colors");
        registerBool(showRaySearchCount, "Debug", "Show ray search count");
        registerBool(showRayTestCount, "Debug", "Show ray test count");
    }
} // Metal
