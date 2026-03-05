#ifndef MATERIALIMPORTERSERVICE_H
#define MATERIALIMPORTERSERVICE_H

#include <assimp/scene.h>
#include <unordered_map>
#include <string>
#include <stop_token>

#include "../abstract/AbstractResourceService.h"

namespace Metal {
    struct MaterialData {
        std::string albedo;
        std::string normal;
        std::string roughness;
        std::string metallic;
    };

    class MaterialImporterService final : public AbstractRuntimeComponent {
    public:

        void persistAllMaterials(const std::string &targetDir, const aiScene *scene,
                                 std::unordered_map<unsigned int, MaterialData> &materialMap,
                                 const std::string &rootDirectory,
                                 const std::stop_token &stopToken = {}) const;
    };
}

#endif // MATERIALIMPORTERSERVICE_H
