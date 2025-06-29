#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../common/AbstractResourceService.h"
#include "../../common/engine-definitions.h"

namespace Metal {
    struct LevelOfDetail;
    struct MeshData;

    class MeshService final : public AbstractResourceService {
    public:

        MeshData *stream(const std::string &id, const LevelOfDetail &levelOfDetail) const;

        [[nodiscard]] EntityID createMeshEntity(const std::string &name, const std::string &meshId) const;

        void createSceneEntities(const std::string &id) const;
    };
} // Metal

#endif //MESHSERVICE_H
