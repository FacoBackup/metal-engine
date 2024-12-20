#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H

#include <assimp/scene.h>

#include "AbstractImporter.h"

namespace Metal {
    struct LevelOfDetail;
    struct MeshData;
    struct SceneData;

    class SceneImporter final : public AbstractImporter {
        void simplifyMesh(const std::string &fileId, const MeshData &mesh, const LevelOfDetail &levelOfDetail) const;

        std::vector<std::string> getSupportedTypes() override;

        static void ProcessNode(int &increment, SceneData &scene, const aiNode *node, int parentId,
                                const std::unordered_map<unsigned int, std::string> &meshMap,
                                const std::unordered_map<std::string, unsigned int> &meshMaterialMap,
                                const std::unordered_map<unsigned int, std::string> &materialsMap);

        void persistAllMeshes(const std::string &targetDir, const aiScene *scene,
                              std::unordered_map<unsigned int, std::string> &meshMap,
                              std::unordered_map<std::string, unsigned int> &meshMaterialMap) const;

        void persistAllMaterials(const std::string &targetDir, const aiScene *scene,
                                 std::unordered_map<unsigned int, std::string> &materialMap,
                                const std::string& rootDirectory) const;

        [[nodiscard]] std::string persistMesh(const std::string &targetDir,
                                              const MeshData &mesh) const;

    public:
        explicit SceneImporter(ApplicationContext &context)
            : AbstractImporter(context) {
        }

        void importScene(const std::string &targetDir, const std::string &pathToFile) const;

        static void serializeMesh(const MeshData &simplifiedMesh, const std::string &newPath);
    };
} // Metal

#endif //MESHIMPORTER_H
