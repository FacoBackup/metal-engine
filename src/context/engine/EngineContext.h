#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <chrono>
#include <vector>

#include "../../dto/ubo/GlobalDataUBO.h"
#include "../../dto/ubo/PPSettingsUBO.h"
#include "../../common/AbstractRuntimeComponent.h"
#include "render-pass/AbstractRenderPass.h"

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

        void updateGlobalData();

        void onSync() override;

        static glm::vec3 CalculateSunColor(float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                                          glm::vec3 &middayColor);

        static glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t);
    };
}
#endif
