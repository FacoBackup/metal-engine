#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H
#include <string>

#include "AbstractImporter.h"

namespace Metal {
    struct MeshData;

    class MeshImporter final : public AbstractImporter {
        static std::vector<MeshData> ImportMeshes(const std::string &pathToFile);

    public:
        explicit MeshImporter(ApplicationContext &context)
            : AbstractImporter(context) {
        }

        void importMesh(const std::string &targetDir, const std::string &pathToFile);
    };
} // Metal

#endif //MESHIMPORTER_H
