#include "VoxelizationRepository.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    const char *VoxelizationRepository::getTitle() {
        return "Debug Voxelization";
    }

    const char *VoxelizationRepository::getIcon() {
        return Icons::apps.c_str();
    }

    void VoxelizationRepository::registerFields() {
        registerInt(levelOfDetail, "", "Level of detail", 2, 15);
        registerBool(randomColors, "", "Random colors");
        registerBool(showRaySearchCount, "", "Show ray search count");
        registerBool(showRayTestCount, "", "Show ray test count");
    }
} // Metal
