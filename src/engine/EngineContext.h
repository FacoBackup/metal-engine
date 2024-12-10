#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "repository/RuntimeRepository.h"
#include "repository/camera/CameraRepository.h"
#include "../context/repository/ResourceRepository.h"
#include "service/camera/CameraMovementService.h"
#include "system/RenderPassSystem.h"
#include "system/camera/CameraSystem.h"
#include "../context/repository/GlobalDataUBO.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        GlobalDataUBO globalDataUBO{};
        long long start = -1;
    public:
        explicit EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        float deltaTime = 0;
        bool globalDataNeedsUpdate = true;

        // ----------- SYSTEMS / SERVICE
        CameraMovementService cameraMovementService{context};
        CameraSystem cameraSystem{context};
        RenderPassSystem renderPassSystem{context};
        // ----------- SYSTEMS

        // ----------- REPOSITORIES
        CameraRepository cameraRepository{context};
        ResourceRepository resourceRepository{};
        RuntimeRepository runtimeRepository{};
        // ----------- REPOSITORIES

        void dispose() const;

        void onInitialize() override;

        void onSync() override;

        ResourceRepository &getResourceRepository();
    };
}
#endif
