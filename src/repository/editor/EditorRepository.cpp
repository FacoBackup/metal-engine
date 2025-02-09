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
        registerColor(accentColor, "", "Accent color");
        registerBool(showVoxels, "Voxels", "Show?");
        registerInt(voxelSearchCount, "Voxels", "Debug count divisor");
        registerBool(showRaySearchCountVoxels, "Voxels", "Show ray search count");
        registerBool(showRayTestCountVoxels, "Voxels", "Show ray test count");

        registerBool(showIcons, "Icons", "Show icons?");

        registerBool(showGrid, "Grid", "Enabled?");
        registerBool(gridOverlayObjects, "Grid", "Overlay objects?");
        registerFloat(gridScale, "Grid", "Scale");
        registerInt(gridThreshold, "Grid", "Threshold");
        registerFloat(gridThickness, "Grid", "Thickness");
    }
}
