#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H

#include <assimp/scene.h>

#include "common/AbstractImporter.h"
#include <unordered_map>
#include <stop_token>

namespace Metal {
    class DirectoryService;
    struct MeshId;
    struct MaterialData;
    struct MeshData;
    struct SceneData;

    class MeshImporterService;
    class MaterialImporterService;

    class SceneImporterService final : public AbstractImporter {
        MeshImporterService *meshImporterService = nullptr;
        MaterialImporterService *materialImporterService = nullptr;
        DirectoryService *directoryService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"MeshImporterService", &meshImporterService},
                {"MaterialImporterService", &materialImporterService},
                {"DirectoryService", &directoryService}
            };
        }

        std::string importData(const std::string &targetDir, const std::string &pathToFile,
                               const std::shared_ptr<ImportSettingsDTO> &settings,
                               const std::stop_token &stopToken) override;

        void processNode(SceneData &scene, const aiScene *aiScene, const aiNode *node,
                         const std::string &targetDir, const std::string &rootDirectory,
                         const std::unordered_map<unsigned int, MeshId> &meshMap,
                         const std::stop_token &stopToken) const;

        void processMeshes(SceneData &scene, const aiScene *aiScene, const aiNode *node,
                           const std::string &targetDir, const std::string &rootDirectory,
                           const std::unordered_map<unsigned int, MeshId> &meshMap,
                           const std::stop_token &stopToken) const;

        static void ProcessLights(SceneData &scene, const aiScene *aiScene);

        std::vector<std::string> getSupportedTypes() override;
    };
} // Metal

#endif //MESHIMPORTER_H
