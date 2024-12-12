#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct MeshData;
    struct MeshInstance;

    class MeshService final : public AbstractResourceService {
    public:
        explicit MeshService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] MeshInstance *createMesh(const MeshData &data) const;

        void dispose(MeshInstance *instance) const;
    };
} // Metal

#endif //MESHSERVICE_H
