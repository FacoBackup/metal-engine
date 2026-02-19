#ifndef MATERIALIMPORTERSERVICE_H
#define MATERIALIMPORTERSERVICE_H

#include <assimp/scene.h>
#include <unordered_map>
#include <string>
#include <stop_token>

#include "../abstract/AbstractResourceService.h"

namespace Metal {
    class MaterialImporterService final : public AbstractResourceService {
    public:
        explicit MaterialImporterService()
            : AbstractResourceService() {
        }

        void persistAllMaterials(const std::string &targetDir, const aiScene *scene,
                                 std::unordered_map<unsigned int, std::string> &materialMap,
                                 const std::string &rootDirectory,
                                 const std::stop_token &stopToken = {}) const;
    };
}

#endif // MATERIALIMPORTERSERVICE_H
