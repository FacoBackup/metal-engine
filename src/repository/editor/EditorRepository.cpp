#include "EditorRepository.h"
#include "../../common/interface/Icons.h"

namespace Metal {
     const char * EditorRepository::getTitle() {
        return "Editor";
     }

    const char *EditorRepository::getIcon(){
         return Icons::settings.c_str();
     }

     void EditorRepository::registerFields() {
        registerBool(randomColorsVoxels, "Voxels", "Random colors");
        registerBool(showRaySearchCountVoxels, "Voxels", "Show ray search count");
        registerBool(showRayTestCountVoxels, "Voxels", "Show ray test count");

         registerFloat(iconScale, "Icons", "Size");
         registerColor(iconColor, "Icons", "Base color");
    }
}