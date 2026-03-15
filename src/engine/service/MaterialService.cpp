#include "MaterialService.h"

#include "../../common/serialization-definitions.h"
#include "../dto/MeshMetadata.h"
#include "TextureService.h"
#include "../dto/PrimitiveComponent.h"

namespace Metal {

    void MaterialService::load(MeshMetadata &materialData, PrimitiveComponent &data) {
        materialData.albedo = data.albedoColor;
        materialData.roughness = data.roughnessFactor;
        materialData.metallic = data.metallicFactor;
        materialData.transmission = data.transmissionFactor;
        materialData.thickness = data.thicknessFactor;
        materialData.ior = data.ior;
        materialData.isEmissive = data.isEmissive ? 1 : 0;

        materialData.albedoTextureId = 0;
        materialData.roughnessTextureId = 0;
        materialData.metallicTextureId = 0;

        if (!data.albedo.empty()) {
            auto *tex = textureService->stream(data.albedo);
            if (tex != nullptr) {
                materialData.albedoTextureId = textureService->getTextureIndex(data.albedo);
            }
        }
        if (!data.roughness.empty()) {
            auto *tex = textureService->stream(data.roughness);
            if (tex != nullptr) {
                materialData.roughnessTextureId = textureService->getTextureIndex(data.roughness);
            }
        }
        if (!data.metallic.empty()) {
            auto *tex = textureService->stream(data.metallic);
            if (tex != nullptr) {
                materialData.metallicTextureId = textureService->getTextureIndex(data.metallic);
            }
        }
    }
} // Metal
