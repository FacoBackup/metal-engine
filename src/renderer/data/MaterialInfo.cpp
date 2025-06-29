#include "MaterialInfo.h"
#include "../../common/enum/Icons.h"

namespace Metal {
    void MaterialInfo::registerFields() {
        registerColor(baseColor, "", "Base color / Absorption");
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

        registerFloat(transmission, "", "Transmission", 0, 1);
        registerFloat(ior, "", "IOR (water = 1.33)", 1);
        registerVec3(absorption, "", "Absorption");
    }

    const char *MaterialInfo::getIcon() {
        return Icons::format_paint.c_str();
    }

    const char *MaterialInfo::getTitle() {
        return "Material";
    }
}
