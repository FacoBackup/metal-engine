#ifndef ACCUMULATIONPASS_H
#define ACCUMULATIONPASS_H
#include "../AbstractComputePass.h"
#include "../../../../dto/push-constant/AccumulationPushConstant.h"

namespace Metal {
    class AccumulationPass final : public AbstractComputePass {
        AccumulationPushConstant pushConstant{};
        bool initialized = false;

    public:
        explicit AccumulationPass()
            : AbstractComputePass() {
        }

        void onSync() override;

        void onInitialize() override;

    };
} // Metal

#endif //ACCUMULATIONPASS_H
