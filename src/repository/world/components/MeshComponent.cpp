#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../enum/EntryType.h"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH, false);
        registerResourceSelection(albedo, "", "Albedo Texture", EntryType::TEXTURE);
        registerBool(useAlbedoTexture, "", "Use Albedo Texture?");
        registerResourceSelection(normal, "", "normal", EntryType::TEXTURE);
        registerBool(useNormalTexture, "", "Use Normal Texture?");
        registerResourceSelection(roughness, "", "Roughness texture", EntryType::TEXTURE);
        registerBool(useRoughnessTexture, "", "Use Roughness Texture?");
        registerResourceSelection(metallic, "", "Metallic Texture", EntryType::TEXTURE);
        registerBool(useMetallicTexture, "", "Use Metallic Texture?");
        registerResourceSelection(ao, "", "Ambient occlusion Texture", EntryType::TEXTURE);
        registerBool(useAOTexture, "", "Use AO Texture?");
        registerResourceSelection(height, "", "Height Texture", EntryType::TEXTURE);
        registerBool(useHeightTexture, "", "Use Height Texture?");
        registerColor(albedoColor, "", "Albedo Color");
        registerBool(emissiveSurface, "", "Is emissive surface?");
        registerFloat(roughnessFactor, "", "Roughness Factor", 0, 1);
        registerFloat(metallicFactor, "", "Metallic Factor", 0, 1);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
