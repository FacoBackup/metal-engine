#ifndef TEMPORALACCUMULATIONPASS_H
#define TEMPORALACCUMULATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class TemporalAccumulationPass final : public AbstractComputePass {
    public:

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //TEMPORALACCUMULATIONPASS_H
