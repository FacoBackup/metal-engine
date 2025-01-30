#ifndef DENOISERPASS_H
#define DENOISERPASS_H
#include "../AbstractComputePass.h"
#include "../../../../dto/push-constant/DenoiserPushConstant.h"

namespace Metal {
    class DenoiserPass final : public AbstractComputePass {
        DenoiserPushConstant pushConstant{};
    public:
        explicit DenoiserPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //DENOISERPASS_H
