#ifndef VOLUMETRICPATHTRACER_H
#define VOLUMETRICPATHTRACER_H
#include "../AbstractComputePass.h"

namespace Metal {

    class VolumetricPathTracer final : public AbstractComputePass {
    public:
        explicit VolumetricPathTracer(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        bool shouldRun() override;
        void onSync() override;

        void onInitialize() override;
    };
}

#endif
