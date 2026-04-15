#include "MaterialImporterService.h"
#include "TextureImporterService.h"
#include "common/LoggerUtil.h"
#include "../enum/engine-definitions.h"
#include "engine/dto/StaticGeometryComponent.h"
#include "engine/dto/Material.h"
#include <assimp/material.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Metal {
    void MaterialImporterService::importMaterial(const std::string &targetDir, const aiMaterial *material,
                                                         const aiScene *scene, const std::string &rootDirectory,
                                                         StaticGeometryComponent &primitive,
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
                        return textureImporterService->importEmbeddedTexture(
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
                return textureImporterService->importData(targetDir, resolved.string(), nullptr, stopToken);
            } catch (std::exception &e) {
                LOG_ERROR("Failed to import texture " + resolved.string() + ": " + e.what());
                return "";
            }
        };

        const auto trySetFromType = [&](std::string &slot, aiTextureType type, const std::string &nameHint) {
            if (material->GetTextureCount(type) == 0) return;
            aiString texturePath;
            if (material->GetTexture(type, 0, &texturePath) == AI_SUCCESS) {
                slot = importAssimpTexture(texturePath, nameHint);
            }
        };

        Material mat;
        // Albedo
        trySetFromType(mat.albedo, aiTextureType_BASE_COLOR, "albedo");
        trySetFromType(mat.albedo, aiTextureType_DIFFUSE, "albedo");

        // Metallic / Roughness
        trySetFromType(mat.metallic, aiTextureType_METALNESS, "metallic");
        trySetFromType(mat.roughness, aiTextureType_DIFFUSE_ROUGHNESS, "roughness");
        
        // Emissive
        trySetFromType(mat.emissive, aiTextureType_EMISSIVE, "emissive");
        
        // Normal
        trySetFromType(mat.normal, aiTextureType_NORMALS, "normal");

        aiString matName;
        material->Get(AI_MATKEY_NAME, matName);
        std::string name = matName.C_Str();
        if (name.empty()) name = "material";
        
        std::string materialPath = (fs::path(targetDir) / (name + ".mmat")).string();
        std::ofstream os(materialPath);
        if (os.is_open()) {
            os << mat.toJson().dump(4);
            primitive.materialId = materialPath;
        }
    }
}
