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
        registerBool(showIcons, "", "Show icons?");
        registerBool(showGrid, "", "Show grid?");
        registerFloat(gridScale, "", "Grid Scale");
        registerInt(gridThreshold, "", "Grid Threshold");
        registerFloat(gridThickness, "", "Grid Thickness");
    }
}
