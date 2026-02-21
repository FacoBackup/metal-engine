#ifndef SELECTEDDOTPASS_H
#define SELECTEDDOTPASS_H

#include "../../AbstractRenderPass.h"
#include "../../../../../dto/push-constant/SelectedDotPushConstant.h"

namespace Metal {
    class SelectedDotPass final : public AbstractRenderPass {
        SelectedDotPushConstant pushConstant{};

    public:
        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // SELECTEDDOTPASS_H
