#ifndef VolumetricPass_H
#define VolumetricPass_H
#include "../AbstractComputePass.h"

namespace Metal {
    class VolumetricPass final : public AbstractComputePass {
    public:
        explicit VolumetricPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //VolumetricPass_H
