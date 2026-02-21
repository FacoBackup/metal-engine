#ifndef VOXELVISUALIZERRENDERPASS_H
#define VOXELVISUALIZERRENDERPASS_H
#include "../AbstractRenderPass.h"
#include "../../../../dto/push-constant/MeshPushConstant.h"

namespace Metal {
    class GBufferGenPass final : public AbstractRenderPass {
        MeshPushConstant mPushConstant{};
    public:

        void onSync() override;

        void onInitialize() override;
    };
}

#endif
