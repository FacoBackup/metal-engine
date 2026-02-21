#ifndef DENOISERPASS_H
#define DENOISERPASS_H
#include "../AbstractComputePass.h"
#include "../../../../dto/push-constant/DenoiserPushConstant.h"

namespace Metal {
    class AccumulationMetadataPass final : public AbstractComputePass {
        DenoiserPushConstant pushConstant{};
    public:
        explicit AccumulationMetadataPass()
            : AbstractComputePass() {
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //DENOISERPASS_H
