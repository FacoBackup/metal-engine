#ifndef VOXELIZERSERVICE_H
#define VOXELIZERSERVICE_H
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    class VoxelizerService final : public AbstractRuntimeComponent {
    public:
        explicit VoxelizerService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void voxelizeScene();
    };
} // Metal

#endif //VOXELIZERSERVICE_H
