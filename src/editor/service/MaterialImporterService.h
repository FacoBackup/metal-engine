#ifndef MATERIALIMPORTERSERVICE_H
#define MATERIALIMPORTERSERVICE_H

#include <assimp/scene.h>
#include <string>
#include <stop_token>

#include "common/IService.h"

namespace Metal {
    struct StaticGeometryComponent;

    class TextureImporterService;

    class MaterialImporterService final : public IService {
        TextureImporterService *textureImporterService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TextureImporterService", &textureImporterService}
            };
        }

        void importMaterial(const std::string &targetDir, const aiMaterial *material, const aiScene *scene,
                            const std::string &rootDirectory, StaticGeometryComponent &primitive,
                            const std::stop_token &stopToken = {}) const;
    };
}

#endif // MATERIALIMPORTERSERVICE_H
