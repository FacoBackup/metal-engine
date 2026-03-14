#ifndef SPATIALFILTERPASS_H
#define SPATIALFILTERPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class SpatialFilterPass final : public AbstractComputePass {
    public:
        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //SPATIALFILTERPASS_H
