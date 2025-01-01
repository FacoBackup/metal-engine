#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../enum/EntryType.h"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH, false);
        registerResourceSelection(albedo, "Textures", "Albedo Texture", EntryType::TEXTURE);
        registerBool(useAlbedoTexture, "Textures", "Use Albedo Texture?");
        registerResourceSelection(normal, "Textures", "normal", EntryType::TEXTURE);
        registerBool(useNormalTexture, "Textures", "Use Normal Texture?");
        registerResourceSelection(roughness, "Textures", "Roughness texture", EntryType::TEXTURE);
        registerBool(useRoughnessTexture, "Textures", "Use Roughness Texture?");
        registerResourceSelection(metallic, "Textures", "Metallic Texture", EntryType::TEXTURE);
        registerBool(useMetallicTexture, "Textures", "Use Metallic Texture?");
        registerResourceSelection(ao, "Textures", "Ambient occlusion Texture", EntryType::TEXTURE);
        registerBool(useAOTexture, "Textures", "Use AO Texture?");
        registerResourceSelection(height, "Textures", "Height Texture", EntryType::TEXTURE);
        registerBool(useHeightTexture, "Textures", "Use Height Texture?");
        registerColor(albedoColor, "", "Albedo Color");
        registerBool(emissiveSurface, "", "Is emissive surface?");
        registerFloat(roughnessFactor, "", "Roughness Factor", 0, 1, false, .001);
        registerFloat(metallicFactor, "", "Metallic Factor", 0, 1, false, .001);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
