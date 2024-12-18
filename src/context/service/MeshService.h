#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../common/interface/AbstractResourceService.h"

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
    };
} // Metal

#endif //MESHSERVICE_H
