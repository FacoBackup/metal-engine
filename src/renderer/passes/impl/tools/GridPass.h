#ifndef GRIDRENDERPASS_H
#define GRIDRENDERPASS_H
#include "../../AbstractRenderPass.h"
#include "../../../data/GridPC.h"

namespace Metal {
    class GridPass final : public AbstractRenderPass {
        GridPC pushConstant{};

    public:
        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //GRIDRENDERPASS_H
