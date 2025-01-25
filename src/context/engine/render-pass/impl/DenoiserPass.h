#ifndef DENOISERPASS_H
#define DENOISERPASS_H
#include "../AbstractRenderPass.h"
#include "../../../../dto/push-constant/DenoiserPushConstant.h"

namespace Metal {
    class DenoiserPass final : public AbstractRenderPass {
        DenoiserPushConstant pushConstant{};
    public:
        explicit DenoiserPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //DENOISERPASS_H
