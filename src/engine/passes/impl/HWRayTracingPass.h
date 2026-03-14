#ifndef HWRAYTRACINGPASS_H
#define HWRAYTRACINGPASS_H
#include "../AbstractComputePass.h"
#include "../../dto/HWRayTracingPushConstant.h"

namespace Metal {
    class HWRayTracingPass final : public AbstractComputePass {
        bool isFirstRun = true;
        HWRayTracingPushConstant pushConstant{};

    public:
        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //HWRAYTRACINGPASS_H
