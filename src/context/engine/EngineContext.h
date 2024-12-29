#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <chrono>
#include <vector>

#include "../../dto/ubo/GlobalDataUBO.h"
#include "../../dto/ubo/PPSettingsUBO.h"
#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/ubo/LightData.h"
#include "../../dto/ubo/TileInfoUBO.h"
#include "render-pass/AbstractRenderPass.h"
#include "../../enum/engine-definitions.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        GlobalDataUBO globalDataUBO{};
        PPSettingsUBO postProcessingUBO{};
        TileInfoUBO tileInfoUBO{};
        std::array<LightData, MAX_LIGHTS> lights{};
        unsigned int lightsCount = 0;
        long long start = -1;
        bool hasToUpdateLights = true;

        std::vector<std::unique_ptr<AbstractRenderPass>> aoPass;
        std::vector<std::unique_ptr<AbstractRenderPass> > fullScreenRenderPasses;
        std::vector<std::unique_ptr<AbstractRenderPass> > gBufferPasses;
        std::vector<std::unique_ptr<AbstractRenderPass>> postProcessingPasses;

    public:

        void setUpdateLights() {
            hasToUpdateLights = true;
        }

        void onInitialize() override;

        void updatePostProcessingData();

        void updateVoxelData();

        explicit EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        long long currentTimeMs = 0;
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        float deltaTime = 0;

        void updateGlobalData();

        void onSync() override;

        void updateLights();

        static glm::vec3 CalculateSunColor(float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                           glm::vec3 &middayColor);

        static glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t);
    };
}
#endif
