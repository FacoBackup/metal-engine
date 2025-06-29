#ifndef DENOISERPASS_H
#define DENOISERPASS_H
#include "../AbstractComputePass.h"
#include "../../data/DenoiserPC.h"

namespace Metal {
    class AccumulationMetadataPass final : public AbstractComputePass {
        DenoiserPC pushConstant{};
    public:

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //DENOISERPASS_H
