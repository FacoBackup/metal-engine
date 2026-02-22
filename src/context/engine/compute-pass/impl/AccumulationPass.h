#ifndef ACCUMULATIONPASS_H
#define ACCUMULATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class AccumulationPass final : public AbstractComputePass {
    public:
        void onSync() override;

        void onInitialize() override;

    };
} // Metal

#endif //ACCUMULATIONPASS_H
