#ifndef SELECTIONIDPASS_H
#define SELECTIONIDPASS_H

#include "../../AbstractRenderPass.h"
#include "../../../../../dto/push-constant/SelectedDotPushConstant.h"

namespace Metal {
    class SelectionIDPass final : public AbstractRenderPass {
        SelectedDotPushConstant pushConstant{};

    public:
        explicit SelectionIDPass(const std::string &id) : AbstractRenderPass(id) {}

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // SELECTIONIDPASS_H
