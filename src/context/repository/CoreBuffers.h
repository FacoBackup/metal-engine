#ifndef COREBUFFERS_H
#define COREBUFFERS_H
#include "AbstractCoreRepository.h"

namespace Metal {
    struct BufferInstance;

    struct CoreBuffers final : AbstractCoreRepository {
        explicit CoreBuffers(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        BufferInstance *globalData = nullptr;
    };
} // Metal

#endif //COREBUFFERS_H
