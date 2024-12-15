#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "repository/RuntimeRepository.h"
#include "repository/camera/CameraRepository.h"
#include "service/camera/CameraMovementService.h"
#include "system/camera/CameraSystem.h"
#include "../context/repository/GlobalDataUBO.h"
#include "render-pass/AbstractRenderPass.h"

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

        // ----------- SYSTEMS / SERVICE
        CameraMovementService cameraMovementService{context};
        CameraSystem cameraSystem{context};
        // ----------- SYSTEMS

        // ----------- REPOSITORIES
        CameraRepository cameraRepository{context};
        RuntimeRepository runtimeRepository{};
        // ----------- REPOSITORIES

        void onSync() override;
    };
}
#endif
