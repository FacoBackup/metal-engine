#ifndef SELECTIONIDPASS_H
#define SELECTIONIDPASS_H

#include "../../engine/passes/AbstractRenderPass.h"
#include "../dto/SelectedDotPushConstant.h"

namespace Metal {
    class SelectionIDPass final : public AbstractRenderPass {
        SelectedDotPushConstant pushConstant{};

    public:

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // SELECTIONIDPASS_H
