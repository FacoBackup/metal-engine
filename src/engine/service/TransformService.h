#ifndef TRANSFORMSERVICE_H
#define TRANSFORMSERVICE_H
#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class TransformService final : AbstractRuntimeComponent {
    public:
        explicit TransformService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void onSync() override;
    };
} // Metal

#endif
