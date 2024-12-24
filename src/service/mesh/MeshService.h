#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../service/abstract/AbstractResourceService.h"
#include "../../enum/engine-definitions.h"

namespace Metal {
    struct LevelOfDetail;
    struct MeshData;
    struct MeshInstance;

    class MeshService final : public AbstractResourceService {
    public:
        explicit MeshService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        MeshInstance *create(const std::string &id, const LevelOfDetail &levelOfDetail);

        [[nodiscard]] EntityID loadMesh(const std::string &name, const std::string &meshId) const;

        void loadScene(const std::string &id) const;
    };
} // Metal

#endif //MESHSERVICE_H