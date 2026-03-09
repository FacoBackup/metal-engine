#include "MaterialImporterService.h"
#include "../../context/ApplicationContext.h"
#include "../../enum/engine-definitions.h"
#include "../../repository/world/components/PrimitiveComponent.h"
#include <assimp/material.h>
#include <filesystem>

namespace Metal {
    void MaterialImporterService::importMaterial(const std::string &targetDir, const aiMaterial *material,
                                                         const aiScene *scene, const std::string &rootDirectory,
                                                         PrimitiveComponent &primitive,
                                                         const std::stop_token &stopToken) const {
        namespace fs = std::filesystem;

        const auto importAssimpTexture = [&](const aiString &assimpPath, const std::string &nameHint) -> std::string {
            if (assimpPath.length == 0) return "";
            const std::string p = assimpPath.C_Str();

            // Embedded textures are referenced as "*<index>".
            if (!p.empty() && p[0] == '*') {
                try {
                    const unsigned int embeddedIndex = static_cast<unsigned int>(std::stoul(p.substr(1)));
                    if (scene && embeddedIndex < scene->mNumTextures) {
                        return CTX.textureImporter.importEmbeddedTexture(
                            targetDir, scene->mTextures[embeddedIndex], nameHint);
                    }
                } catch (std::exception &e) {
                    LOG_ERROR("Failed to import texture: " + std::string(e.what()));
                    return "";
                }
                return "";
            }

            fs::path resolved = fs::path(p);
            if (!resolved.is_absolute()) {
                resolved = fs::path(rootDirectory) / resolved;
            }
            resolved = resolved.lexically_normal();
            try {
                return CTX.textureImporter.importData(targetDir, resolved.string(), nullptr, stopToken);
            } catch (std::exception &e) {
                LOG_ERROR("Failed to import texture " + resolved.string() + ": " + e.what());
                return "";
            }
        };

        const auto trySetFromType = [&](std::string &slot, aiTextureType type, const std::string &nameHint) {
            if (!slot.empty()) return;
            if (material->GetTextureCount(type) == 0) return;
            aiString texturePath;
            if (material->GetTexture(type, 0, &texturePath) == AI_SUCCESS) {
                slot = importAssimpTexture(texturePath, nameHint);
            }
        };

        // Albedo
        trySetFromType(primitive.albedo, aiTextureType_BASE_COLOR, "albedo");
        trySetFromType(primitive.albedo, aiTextureType_DIFFUSE, "albedo");

        // Metallic / Roughness
        trySetFromType(primitive.metallic, aiTextureType_METALNESS, "metallic");
        trySetFromType(primitive.roughness, aiTextureType_DIFFUSE_ROUGHNESS, "roughness");

        // Factors & Colors
        aiColor4D color;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            primitive.albedoColor = {color.r, color.g, color.b};
        }

        if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
            primitive.isEmissive = (color.r > 0 || color.g > 0 || color.b > 0);
        }

        // float factor;
        // if (material->Get(AI_MATKEY_METALLIC_FACTOR, factor) == AI_SUCCESS) {
        //     primitive.metallicFactor = factor;
        // }
        //
        // if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, factor) == AI_SUCCESS) {
        //     primitive.roughnessFactor = factor;
        // }
        //
        // if (material->Get(AI_MATKEY_TRANSMISSION_FACTOR, factor) == AI_SUCCESS) {
        //     primitive.transmissionFactor = factor;
        // }
        //
        // if (material->Get(AI_MATKEY_REFRACTI, factor) == AI_SUCCESS) {
        //     primitive.ior = factor;
        // }
    }
}
