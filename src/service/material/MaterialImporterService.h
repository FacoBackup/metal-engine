#ifndef MATERIALIMPORTERSERVICE_H
#define MATERIALIMPORTERSERVICE_H

#include "../abstract/AbstractImporter.h"
#include <assimp/scene.h>
#include <unordered_map>
#include <string>
#include <stop_token>

namespace Metal {
    class MaterialImporterService final : public AbstractImporter {
    public:
        explicit MaterialImporterService(ApplicationContext &context)
            : AbstractImporter(context) {}

        void persistAllMaterials(const std::string &targetDir, const aiScene *scene,
                                 std::unordered_map<unsigned int, std::string> &materialMap,
                                 const std::string &rootDirectory,
                                 const std::stop_token &stopToken = {}) const;
    };
}

#endif // MATERIALIMPORTERSERVICE_H
