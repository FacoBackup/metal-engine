#ifndef SELECTEDDOTPASS_H
#define SELECTEDDOTPASS_H

#include "../../AbstractRenderPass.h"
#include "../../../../../dto/push-constant/MeshPushConstant.h"

namespace Metal {
    class SelectedDotPass final : public AbstractRenderPass {
        MeshPushConstant pushConstant{};

    public:
        explicit SelectedDotPass()
            : AbstractRenderPass() {
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // SELECTEDDOTPASS_H
