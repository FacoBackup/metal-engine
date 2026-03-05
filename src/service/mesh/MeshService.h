#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../service/abstract/AbstractResourceService.h"
#include "../../service/abstract/IStreamable.h"
#include "MeshInstance.h"
#include "../../enum/engine-definitions.h"

namespace Metal {
    struct MeshData;
    struct MeshInstance;
    struct EntityAssetData;

    class MeshService final : public IStreamable<MeshInstance> {
    public:
        MeshInstance *create(const std::string &id) override;

        MeshData *stream(const std::string &id) const;

        EntityID createMeshEntity(const std::string &name, const std::string &meshId,
                                  const EntityAssetData *data) const;

        void createSceneEntities(const std::string &id) const;

        void disposeResource(MeshInstance *resource) override;

    };
} // Metal

#endif //MESHSERVICE_H
