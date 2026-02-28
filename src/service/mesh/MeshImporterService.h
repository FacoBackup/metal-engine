#ifndef MESHIMPORTERSERVICE_H
#define MESHIMPORTERSERVICE_H

#include "../abstract/AbstractResourceService.h"
#include <assimp/scene.h>
#include <unordered_map>
#include <string>
#include <stop_token>

namespace Metal {
    struct MeshData;

    class MeshImporterService final : public AbstractRuntimeComponent {
    public:

        void persistAllMeshes(const std::string &targetDir, const aiScene *scene,
                              std::unordered_map<unsigned int, std::string> &meshMap,
                              std::unordered_map<std::string, unsigned int> &meshMaterialMap,
                              const std::stop_token &stopToken = {}) const;

    private:
        [[nodiscard]] std::string persistMesh(const std::string &targetDir, const MeshData &mesh) const;
    };
}

#endif // MESHIMPORTERSERVICE_H
