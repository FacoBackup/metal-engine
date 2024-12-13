#ifndef VOXELVISUALIZERRENDERPASS_H
#define VOXELVISUALIZERRENDERPASS_H
#include "../AbstractRenderPass.h"
#include "../../../context/repository/MeshPushConstant.h"

namespace Metal {
    class OpaqueRenderPass final : public AbstractRenderPass {
        MeshPushConstant mPushConstant{};
    public:
        explicit OpaqueRenderPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onSync() override;
    };
}

#endif
