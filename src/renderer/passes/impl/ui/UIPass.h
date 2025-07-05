#ifndef UIPASS_H
#define UIPASS_H

#include "../../AbstractComputePass.h"
#include "../../../data/UserInterfacePC.h"

namespace Metal {
    class UIPass final : public AbstractComputePass {
        UserInterfacePC pushConstant{};
    public:
        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //UIPASS_H
