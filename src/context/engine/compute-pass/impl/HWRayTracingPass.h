#ifndef HWRAYTRACINGPASS_H
#define HWRAYTRACINGPASS_H
#include "../AbstractComputePass.h"
#include "../../../../dto/push-constant/HWRayTracingPushConstant.h"

namespace Metal {
    class HWRayTracingPass final : public AbstractComputePass {
        bool isFirstRun = true;
        HWRayTracingPushConstant pushConstant{};
        TextureInstance *rawRenderedFrame = nullptr;
        TextureInstance *accumulatedFrame = nullptr;
        TextureInstance *gBufferPositionIndex = nullptr;
        TextureInstance *gBufferNormal = nullptr;
        TextureInstance *previousColor = nullptr;
        TextureInstance *previousPositionIndex = nullptr;
        TextureInstance *previousNormal = nullptr;

    public:
        explicit HWRayTracingPass(const std::string &id) : AbstractComputePass(id) {
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //HWRAYTRACINGPASS_H
