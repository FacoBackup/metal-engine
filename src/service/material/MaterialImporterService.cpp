#include "MaterialImporterService.h"
#include "../../context/ApplicationContext.h"
#include "MaterialData.h"
#include "../../dto/file/FileMetadata.h"
#include "../../enum/engine-definitions.h"
#include "../../util/FilesUtil.h"
#include <assimp/material.h>

#include <filesystem>

#include <cereal/archives/binary.hpp>

namespace Metal {
    void MaterialImporterService::persistAllMaterials(const std::string &targetDir, const aiScene *scene,
                                                   std::unordered_map<unsigned int, std::string> &materialMap,
                                                   const std::string &rootDirectory,
                                                   const std::stop_token &stopToken) const {
        namespace fs = std::filesystem;
        LOG_INFO(context, "Processing materials for scene...");
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            if (stopToken.stop_requested()) return;
            const aiMaterial *material = scene->mMaterials[i];
            auto materialData = MaterialData{};

            const auto importAssimpTexture = [&](const aiString &assimpPath, const std::string &nameHint) -> std::string {
                if (assimpPath.length == 0) return "";
                const std::string p = assimpPath.C_Str();

                // Embedded textures are referenced as "*<index>".
                if (!p.empty() && p[0] == '*') {
                    try {
                        const unsigned int embeddedIndex = static_cast<unsigned int>(std::stoul(p.substr(1)));
                        if (scene && embeddedIndex < scene->mNumTextures) {
                            return context.textureImporter.importEmbeddedTexture(targetDir, scene->mTextures[embeddedIndex], nameHint);
                        }
                    } catch (...) {
                        return "";
                    }
                    return "";
                }

                fs::path resolved = fs::path(p);
                if (!resolved.is_absolute()) {
                    resolved = fs::path(rootDirectory) / resolved;
                }
                resolved = resolved.lexically_normal();
                return context.textureImporter.importTexture(targetDir, resolved.string());
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
            trySetFromType(materialData.albedo, aiTextureType_BASE_COLOR, "albedo");
            trySetFromType(materialData.albedo, aiTextureType_DIFFUSE, "albedo");

            // Normal
            trySetFromType(materialData.normal, aiTextureType_NORMALS, "normal");
            trySetFromType(materialData.normal, aiTextureType_NORMAL_CAMERA, "normal");

            // Metallic / Roughness
            trySetFromType(materialData.metallic, aiTextureType_METALNESS, "metallic");
            trySetFromType(materialData.roughness, aiTextureType_DIFFUSE_ROUGHNESS, "roughness");

            // Height (if present)
            trySetFromType(materialData.height, aiTextureType_HEIGHT, "height");

            // If we didn't import any textures, don't create/persist a material at all.
            if (materialData.albedo.empty() && materialData.normal.empty() && materialData.roughness.empty() &&
                materialData.metallic.empty() && materialData.height.empty()) {
                LOG_INFO(context, "Skipping material " + std::to_string(i) + ": no textures associated");
                continue;
            }

            std::string materialId;
            {
                FileMetadata materialMetadata{};
                materialMetadata.type = EntryType::MATERIAL;
                materialMetadata.name = "Material " + std::to_string(i);
                DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(materialMetadata.getId()), materialMetadata)
                materialId = materialMetadata.getId();
            }
            materialMap.insert({i, materialId});

            DUMP_TEMPLATE(context.getAssetDirectory() + FORMAT_FILE_MATERIAL(materialId), materialData)
            LOG_INFO(context, "Persisted material: " + materialId);
        }
    }
}
