#ifndef COREBUFFERS_H
#define COREBUFFERS_H
#include <memory>

#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct BufferInstance;

    struct CoreBuffers final : AbstractRuntimeComponent {
        void onInitialize() override;

        std::shared_ptr<BufferInstance> globalData = nullptr;
        std::shared_ptr<BufferInstance> lightsBuffer = nullptr;
        std::shared_ptr<BufferInstance> volumesBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtTrianglesBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtTLASBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtBLASBuffer = nullptr;
        std::shared_ptr<BufferInstance> rtMaterialData = nullptr;
    };
} // Metal

#endif //COREBUFFERS_H
