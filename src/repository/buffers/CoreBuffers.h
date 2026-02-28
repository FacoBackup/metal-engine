#ifndef COREBUFFERS_H
#define COREBUFFERS_H
#include <memory>

#include "../abstract/AbstractCoreRepository.h"

namespace Metal {
    struct BufferInstance;

    struct CoreBuffers final : AbstractCoreRepository {
        explicit CoreBuffers()
            : AbstractCoreRepository() {
        }

        void onInitialize() override;

        BufferInstance *globalData = nullptr;
        BufferInstance *tileInfo = nullptr;
        BufferInstance *lightBuffer = nullptr;
        BufferInstance *volumesBuffer = nullptr;
        BufferInstance *materialBuffer = nullptr;
    };
} // Metal

#endif //COREBUFFERS_H
