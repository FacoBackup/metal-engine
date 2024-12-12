#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H

#include "AbstractImporter.h"

namespace Metal {
    struct LevelOfDetail;
    struct MeshData;

    class MeshImporter final : public AbstractImporter {
        static std::vector<MeshData> ImportMeshes(const std::string &pathToFile);

        void simplifyMesh(std::vector<std::string> &paths, const std::string &fileId,
                                      const std::string &targetDir,
                                      const MeshData &mesh, const LevelOfDetail &levelOfDetail);

    public:
        explicit MeshImporter(ApplicationContext &context)
            : AbstractImporter(context) {
        }


        void importMesh(const std::string &targetDir, const std::string &pathToFile);
    };
} // Metal

#endif //MESHIMPORTER_H
