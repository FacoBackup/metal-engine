#ifndef HWRAYTRACINGPASS_H
#define HWRAYTRACINGPASS_H
#include "../AbstractComputePass.h"
#include "../../../../dto/push-constant/HWRayTracingPushConstant.h"

namespace Metal {
    class HWRayTracingPass final : public AbstractComputePass {
        bool isFirstRun = true;
        HWRayTracingPushConstant pushConstant{};

    public:
        explicit HWRayTracingPass(const std::string &id) : AbstractComputePass(id) {}

        bool shouldRun() override;

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //HWRAYTRACINGPASS_H
