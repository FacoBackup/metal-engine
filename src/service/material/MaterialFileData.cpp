#include "MaterialFileData.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    void MaterialFileData::registerFields() {
        registerResourceSelection(albedo, "", "Albedo Texture", EntryType::TEXTURE);
        registerResourceSelection(normal, "", "normal", EntryType::TEXTURE);
        registerResourceSelection(roughness, "", "Roughness texture", EntryType::TEXTURE);
        registerResourceSelection(metallic, "", "Metallic Texture", EntryType::TEXTURE);
        registerResourceSelection(height, "", "Height Texture", EntryType::TEXTURE);
    }

    const char *MaterialFileData::getIcon() {
        return Icons::format_paint.c_str();
    }

    const char *MaterialFileData::getTitle() {
        return "Material";
    }
}
