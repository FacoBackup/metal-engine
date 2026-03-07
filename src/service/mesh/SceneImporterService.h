#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H

#include <assimp/scene.h>

#include "../abstract/AbstractImporter.h"
#include <unordered_map>
#include <stop_token>

namespace Metal {
    struct MeshId;
    struct MaterialData;
    struct MeshData;
    struct SceneData;

    class SceneImporterService final : public AbstractImporter {
        static void ProcessNode(SceneData &scene, const aiScene *aiScene, const aiNode *node,
                                const std::string &targetDir, const std::string &rootDirectory,
                                const std::unordered_map<unsigned int, MeshId> &meshMap,
                                const std::stop_token &stopToken);

        static void ProcessMeshes(SceneData &scene, const aiScene *aiScene, const aiNode *node,
                                  const std::string &targetDir, const std::string &rootDirectory,
                                  const std::unordered_map<unsigned int, MeshId> &meshMap,
                                  const std::stop_token &stopToken);

        static void ProcessLights(SceneData &scene, const aiScene *aiScene);

    public:
        std::string importData(const std::string &targetDir, const std::string &pathToFile,
                               const std::shared_ptr<ImportSettingsDTO> &settings,
                               const std::stop_token &stopToken) override;

        std::vector<std::string> getSupportedTypes() override;
    };
} // Metal

#endif //MESHIMPORTER_H
