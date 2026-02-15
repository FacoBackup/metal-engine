#include "MaterialImporterService.h"
#include "../../context/ApplicationContext.h"
#include "MaterialData.h"
#include "../../dto/file/FileMetadata.h"
#include "../../enum/engine-definitions.h"
#include "../../util/FilesUtil.h"
#include <assimp/material.h>

#define TEXTURE_P context.textureImporter.importTexture(targetDir, rootDirectory + "/" + texturePath.data)

#include <cereal/archives/binary.hpp>

namespace Metal {
    void MaterialImporterService::persistAllMaterials(const std::string &targetDir, const aiScene *scene,
                                                   std::unordered_map<unsigned int, std::string> &materialMap,
                                                   const std::string &rootDirectory,
                                                   const std::stop_token &stopToken) const {
        LOG_INFO(context, "Processing materials for scene...");
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            if (stopToken.stop_requested()) return;
            const aiMaterial *material = scene->mMaterials[i];
            std::string materialId; {
                FileMetadata materialMetadata{};
                materialMetadata.type = EntryType::MATERIAL;
                materialMetadata.name = "Material " + std::to_string(i);
                DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(materialMetadata.getId()), materialMetadata)
                materialId = materialMetadata.getId();
            }
            materialMap.insert({i, materialId});

            auto materialData = MaterialData{};
            for (int textureType = aiTextureType_NONE + 1; textureType <= aiTextureType_UNKNOWN; ++textureType) {
                const auto type = static_cast<aiTextureType>(textureType);

                if (unsigned int textureCount = material->GetTextureCount(type); textureCount > 0) {
                    for (unsigned int j = 0; j < textureCount; ++j) {
                        aiString texturePath;
                        if (material->GetTexture(type, j, &texturePath) == AI_SUCCESS) {
                            switch (type) {
                                case aiTextureType_BASE_COLOR: {
                                    materialData.albedo = TEXTURE_P;
                                    break;
                                }
                                case aiTextureType_NORMALS: {
                                    materialData.normal = TEXTURE_P;
                                    break;
                                }
                                case aiTextureType_HEIGHT: {
                                    materialData.height = TEXTURE_P;
                                    break;
                                }
                                case aiTextureType_METALNESS: {
                                    materialData.metallic = TEXTURE_P;
                                    break;
                                }
                                case aiTextureType_DIFFUSE_ROUGHNESS: {
                                    materialData.roughness = TEXTURE_P;
                                    break;
                                }
                                default: break;
                            }
                        }
                    }
                }
            }
            DUMP_TEMPLATE(context.getAssetDirectory() + FORMAT_FILE_MATERIAL(materialId), materialData)
            LOG_INFO(context, "Persisted material: " + materialId);
        }
    }
}
