#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H

#include <assimp/scene.h>

#include "../abstract/AbstractImporter.h"
#include <unordered_map>
#include <stop_token>

namespace Metal {
    struct MaterialData;
    struct MeshData;
    struct SceneData;

    class SceneImporterService final : public AbstractImporter {
        static void ProcessNode(int &increment, SceneData &scene, const aiNode *node, int parentId,
                                const std::unordered_map<unsigned int, std::string> &meshMap,
                                const std::unordered_map<std::string, unsigned int> &meshMaterialMap,
                                const std::unordered_map<unsigned int, MaterialData> &materialsMap,
                                const std::stop_token &stopToken);

    public:
        std::string importData(const std::string &targetDir, const std::string &pathToFile, const std::shared_ptr<ImportSettingsDTO> &settings, const std::stop_token &stopToken) override;

        std::vector<std::string> getSupportedTypes() override;
    };
} // Metal

#endif //MESHIMPORTER_H
