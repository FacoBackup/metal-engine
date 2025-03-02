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
        std::shared_ptr<BufferInstance> lightVolumeBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtTrianglesBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtTLASBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtBLASBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtMaterialData = nullptr;
    };
} // Metal

#endif //COREBUFFERS_H
