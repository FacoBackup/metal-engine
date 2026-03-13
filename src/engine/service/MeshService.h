#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../../common/AbstractResourceService.h"
#include "../resource/MeshInstance.h"
#include "../../editor/enum/engine-definitions.h"

namespace Metal {
    struct MeshData;
    struct MeshInstance;
    struct EntityAssetData;

    class MeshService final : public AbstractResourceService<MeshInstance> {
    public:
        MeshInstance *create(const std::string &id);

        MeshInstance *stream(const std::string &id);

        MeshData *loadMeshData(const std::string &id) const;

        void disposeResource(MeshInstance *resource) override;

    };
} // Metal

#endif //MESHSERVICE_H
