#ifndef SPATIALFILTERPASS_H
#define SPATIALFILTERPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class SpatialFilterPass final : public AbstractComputePass {
    public:
        explicit SpatialFilterPass(const std::string &id) : AbstractComputePass(id) {
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //SPATIALFILTERPASS_H
