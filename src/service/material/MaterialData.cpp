#include "MaterialData.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    void MaterialData::registerFields() {
        registerResourceSelection(albedo, "", "Albedo Texture", EntryType::TEXTURE);
        registerResourceSelection(normal, "", "normal", EntryType::TEXTURE);
        registerResourceSelection(roughness, "", "Roughness texture", EntryType::TEXTURE);
        registerResourceSelection(metallic, "", "Metallic Texture", EntryType::TEXTURE);
        registerResourceSelection(ao, "", "Ambient occlusion Texture", EntryType::TEXTURE);
        registerResourceSelection(height, "", "Height Texture", EntryType::TEXTURE);
    }

    const char *MaterialData::getIcon() {
        return Icons::format_paint.c_str();
    }

    const char *MaterialData::getTitle() {
        return "Material";
    }
}
