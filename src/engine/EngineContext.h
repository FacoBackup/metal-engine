#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "RuntimeRepository.h"
#include "service/camera/CameraService.h"
#include "../context/repository/GlobalDataUBO.h"
#include "render-pass/AbstractRenderPass.h"
#include "service/world/WorldRepository.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        GlobalDataUBO globalDataUBO{};
        long long start = -1;

        std::vector<std::unique_ptr<AbstractRenderPass> > fullScreenRenderPasses;

    public:
        explicit EngineContext(ApplicationContext &context);

        float deltaTime = 0;
        bool globalDataNeedsUpdate = true;

        CameraService cameraService{context};
        WorldRepository worldRepository;
        RuntimeRepository runtimeRepository{};

        void onSync() override;
    };
}
#endif
