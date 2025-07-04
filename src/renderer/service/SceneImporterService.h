#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H

#include <assimp/scene.h>

#include "../../common/AbstractImporter.h"
#include <unordered_map>

namespace Metal {
    struct LevelOfDetail;
    struct MeshData;
    struct SceneData;

    class SceneImporterService final : public AbstractImporter {
        void simplifyMesh(const std::string &fileId, const MeshData &mesh, const LevelOfDetail &levelOfDetail) const;

        std::vector<std::string> getSupportedTypes() override;

        static void ProcessNode(int &increment, SceneData &scene, const aiNode *node, int parentId,
                                const std::unordered_map<unsigned int, std::string> &meshMap);

        void persistAllMeshes(const std::string &targetDir, const aiScene *scene,
                              std::unordered_map<unsigned int, std::string> &meshMap) const;

        [[nodiscard]] std::string persistMesh(const std::string &targetDir,
                                              const MeshData &mesh) const;

    public:

        void importScene(const std::string &targetDir, const std::string &pathToFile) const;
    };
} // Metal

#endif //MESHIMPORTER_H
