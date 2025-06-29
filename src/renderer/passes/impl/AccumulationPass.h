#ifndef ACCUMULATIONPASS_H
#define ACCUMULATIONPASS_H
#include "../AbstractComputePass.h"
#include "../../data/AccumulationPC.h"

namespace Metal {
    class AccumulationPass final : public AbstractComputePass {
        AccumulationPC pushConstant{};
        bool initialized = false;

    public:

        void onSync() override;

        void onInitialize() override;

    };
} // Metal

#endif //ACCUMULATIONPASS_H
