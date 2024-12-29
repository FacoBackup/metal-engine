#include "EditorRepository.h"
#include "../../common/interface/Icons.h"

namespace Metal {
    const char *EditorRepository::getTitle() {
        return "Editor";
    }

    const char *EditorRepository::getIcon() {
        return Icons::settings.c_str();
    }

    void EditorRepository::registerFields() {
        // RANDOM - ALBEDO - NORMAL - ROUGHNESS - METALLIC
        registerInt(voxelDebugFlag, "Voxels", "Random colors", 0, 4);
        registerBool(showRaySearchCountVoxels, "Voxels", "Show ray search count");
        registerBool(showRayTestCountVoxels, "Voxels", "Show ray test count");

        registerFloat(iconScale, "Icons", "Size");
        registerColor(iconColor, "Icons", "Base color");
    }
}
