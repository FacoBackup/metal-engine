#include "./MaterialInfo.h"
#include "../../common/interface/Icons.h"

namespace Metal {
    void MaterialInfo::registerFields() {
        registerColor(baseColor, "", "Base color");
        registerFloat(subsurface, "", "Subsurface");
        registerFloat(roughness, "", "Roughness");
        registerFloat(metallic, "", "Metallic");
        registerFloat(specular, "", "Specular");
        registerFloat(specularTint, "", "Specular tint");
        registerFloat(clearcoat, "", "Clear coat");
        registerFloat(clearcoatGloss, "", "Clear coat gloss");
        registerFloat(anisotropic, "", "Anisotropic");
        registerFloat(sheen, "", "Sheen");
        registerFloat(sheenTint, "", "Sheen tint");
        registerBool(isEmissive, "", "Is emissive?");
    }

    const char *MaterialInfo::getIcon() {
        return Icons::format_paint.c_str();
    }

    const char *MaterialInfo::getTitle() {
        return "Material";
    }
}
