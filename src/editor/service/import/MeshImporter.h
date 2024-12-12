#ifndef MESHIMPORTER_H
#define MESHIMPORTER_H
#include <string>

#include "AbstractImporter.h"

namespace Metal {
    class MeshImporter final : public AbstractImporter {
    public:
        explicit MeshImporter(ApplicationContext &context)
            : AbstractImporter(context) {
        }

        void importMesh(const std::string &pathToFile);
    };
} // Metal

#endif //MESHIMPORTER_H
