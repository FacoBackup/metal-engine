#ifndef COREBUFFERS_H
#define COREBUFFERS_H
#include <memory>

#include "../abstract/AbstractCoreRepository.h"

namespace Metal {
    struct BufferInstance;

    struct CoreBuffers final : AbstractCoreRepository {
        explicit CoreBuffers(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        std::shared_ptr<BufferInstance> globalData = nullptr;
        std::shared_ptr<BufferInstance> tileInfo = nullptr;
        std::shared_ptr<BufferInstance> lights = nullptr;
    };
} // Metal

#endif //COREBUFFERS_H
