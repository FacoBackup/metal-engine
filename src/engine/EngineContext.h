#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "RuntimeRepository.h"
#include "service/camera/CameraService.h"
#include "../context/buffers/GlobalDataUBO.h"
#include "../context/buffers/PPSettingsUBO.h"
#include "render-pass/AbstractRenderPass.h"
#include "service/AtmosphereRepository.h"
#include "service/streaming/StreamingRepository.h"
#include "service/world/WorldRepository.h"
#include "service/world/impl/WorldGridRepository.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        GlobalDataUBO globalDataUBO{};
        PPSettingsUBO postProcessingUBO{};
        long long start = -1;

        std::vector<std::unique_ptr<AbstractRenderPass> > fullScreenRenderPasses;
        std::vector<std::unique_ptr<AbstractRenderPass> > gBufferPasses;
        std::vector<std::unique_ptr<AbstractRenderPass> > postProcessingPasses;

    public:
        explicit EngineContext(ApplicationContext &context);

        long long currentTimeMs = 0;
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        float deltaTime = 0;

        WorldGridRepository worldGridRepository{context};
        CameraService cameraService{context};
        WorldRepository worldRepository{};
        RuntimeRepository runtimeRepository{};
        StreamingRepository streamingRepository{context};
        AtmosphereRepository atmosphereRepository{};

        void updateGlobalData();

        void onSync() override;

        static glm::vec3 CalculateSunColor(float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                                          glm::vec3 &middayColor);

        static glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t);
    };
}
#endif
