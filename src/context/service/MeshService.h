#ifndef MESHSERVICE_H
#define MESHSERVICE_H
#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct MeshData;
    struct MeshInstance;

    class MeshService final : public AbstractResourceService {
    public:
        explicit MeshService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        MeshInstance *createMesh(const std::string &path);

        void dispose(MeshInstance *instance) const;
    };
} // Metal

#endif //MESHSERVICE_H
