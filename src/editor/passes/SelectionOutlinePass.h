#ifndef SELECTEDDOTPASS_H
#define SELECTEDDOTPASS_H

#include "../../engine/passes/AbstractRenderPass.h"
#include "../dto/SelectedDotPushConstant.h"

namespace Metal {
    class SelectionOutlinePass final : public AbstractRenderPass {
        SelectedDotPushConstant pushConstant{};

    public:
        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // SELECTEDDOTPASS_H
