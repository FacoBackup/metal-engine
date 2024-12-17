#ifndef VOXELIZERSERVICE_H
#define VOXELIZERSERVICE_H
#include "../../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class VoxelizerService final : public AbstractRuntimeComponent {
    public:
        explicit VoxelizerService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }
    };
} // Metal

#endif //VOXELIZERSERVICE_H
