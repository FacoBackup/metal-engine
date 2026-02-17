#ifndef HWRAYTRACINGPASS_H
#define HWRAYTRACINGPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class HWRayTracingPass final : public AbstractComputePass {
    public:
        explicit HWRayTracingPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //HWRAYTRACINGPASS_H
