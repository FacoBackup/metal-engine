#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H

#include "AbstractImporter.h"

namespace Metal {
    struct LevelOfDetail;
    struct MeshData;

    class MeshImporter final : public AbstractImporter {
        static std::vector<MeshData> ImportMeshes(const std::string &pathToFile);

        void simplifyMesh(const std::string &fileId, const MeshData &mesh, const LevelOfDetail &levelOfDetail);

        std::vector<std::string> getSupportedTypes() override;

    public:
        explicit MeshImporter(ApplicationContext &context)
            : AbstractImporter(context) {
        }


        void importMesh(const std::string &targetDir, const std::string &pathToFile);

        static void serializeMesh(const MeshData &simplifiedMesh, const std::string &newPath);
    };
} // Metal

#endif //MESHIMPORTER_H
