#ifndef MATERIALIMPORTERSERVICE_H
#define MATERIALIMPORTERSERVICE_H

#include <assimp/scene.h>
#include <unordered_map>
#include <string>
#include <stop_token>

#include "../../common/AbstractResourceService.h"

namespace Metal {
    struct PrimitiveComponent;

    class TextureImporterService;

    class MaterialImporterService final : public AbstractRuntimeComponent {
        TextureImporterService &textureImporterService;
        std::string &rootDirectory;

    public:
        MaterialImporterService(TextureImporterService &textureImporterService, std::string &rootDirectory)
            : textureImporterService(textureImporterService), rootDirectory(rootDirectory) {
        }

        void importMaterial(const std::string &targetDir, const aiMaterial *material, const aiScene *scene,
                            const std::string &rootDirectory, PrimitiveComponent &primitive,
                            const std::stop_token &stopToken = {}) const;
    };
}

#endif // MATERIALIMPORTERSERVICE_H
