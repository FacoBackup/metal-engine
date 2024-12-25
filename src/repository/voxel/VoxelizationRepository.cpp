#include "VoxelizationRepository.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    const char *VoxelizationRepository::getTitle() {
        return "Voxelization";
    }

    const char *VoxelizationRepository::getIcon() {
        return Icons::block.c_str();
    }

    void VoxelizationRepository::registerFields() {
        registerInt(maxDepth, "", "Max depth", 2, 15);
    }
} // Metal
