#ifndef HWRAYTRACINGPASS_H
#define HWRAYTRACINGPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class HWRayTracingPass final : public AbstractComputePass {
        bool isFirstRun = true;

    public:
        explicit HWRayTracingPass()
            : AbstractComputePass() {
        }

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //HWRAYTRACINGPASS_H
