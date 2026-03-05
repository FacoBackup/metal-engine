#include "MaterialService.h"

#include "../../util/serialization-definitions.h"
#include "../../dto/buffers/MeshMetadata.h"

#include "../../context/ApplicationContext.h"


namespace Metal {
    void MaterialService::load(MeshMetadata &materialData, PrimitiveComponent &data) {
        materialData.albedo = data.albedoColor;
        materialData.roughness = data.roughnessFactor;
        materialData.metallic = data.metallicFactor;
        materialData.transmission = data.transmissionFactor;
        materialData.thickness = data.thicknessFactor;
        materialData.ior = data.ior;
        materialData.isEmissive = data.isEmissive ? 1 : 0;

        materialData.useAlbedoTexture = !data.albedo.empty();
        materialData.useNormalTexture = !data.normal.empty();
        materialData.useRoughnessTexture = !data.roughness.empty();
        materialData.useMetallicTexture = !data.metallic.empty();

        materialData.albedoTextureId = 0;
        materialData.normalTextureId = 0;
        materialData.roughnessTextureId = 0;
        materialData.metallicTextureId = 0;

        if (!data.albedo.empty()) {
            auto *tex = CTX.textureService.create(data.albedo);
            if (tex != nullptr) {
                materialData.albedoTextureId = CTX.textureService.getTextureIndex(data.albedo);
            }
        }
        if (!data.normal.empty()) {
            auto *tex = CTX.textureService.create(data.normal);
            if (tex != nullptr) {
                materialData.normalTextureId = CTX.textureService.getTextureIndex(data.normal);
            }
        }
        if (!data.roughness.empty()) {
            auto *tex = CTX.textureService.create(data.roughness);
            if (tex != nullptr) {
                materialData.roughnessTextureId = CTX.textureService.getTextureIndex(data.roughness);
            }
        }
        if (!data.metallic.empty()) {
            auto *tex = CTX.textureService.create(data.metallic);
            if (tex != nullptr) {
                materialData.metallicTextureId = CTX.textureService.getTextureIndex(data.metallic);
            }
        }
    }
} // Metal
