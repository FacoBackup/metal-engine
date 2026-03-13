#ifndef MESHIMPORTERSERVICE_H
#define MESHIMPORTERSERVICE_H

#include "../../common/AbstractResourceService.h"
#include <assimp/scene.h>
#include <unordered_map>
#include <string>
#include <stop_token>
#include <glm/vec3.hpp>

namespace Metal {
    struct MeshId {
        std::string id;
        glm::vec3 gizmoCenter;
    };
    struct MeshData;

    class MeshImporterService final : public AbstractRuntimeComponent {
        std::string &rootDirectory;

    public:
        explicit MeshImporterService(std::string &rootDirectory)
            : rootDirectory(rootDirectory) {
        }

        void persistAllMeshes(const std::string &targetDir, const aiScene *scene,
                              std::unordered_map<unsigned int, MeshId> &meshMap,
                              const std::stop_token &stopToken = {}) const;

    private:
        [[nodiscard]] std::string persistMesh(const std::string &targetDir, const MeshData &mesh) const;
    };
}

#endif // MESHIMPORTERSERVICE_H
