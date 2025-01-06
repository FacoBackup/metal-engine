#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <chrono>

#include "../../dto/ubo/GlobalDataUBO.h"
#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/ubo/LightData.h"
#include "../../dto/ubo/TileInfoUBO.h"
#include "../../enum/engine-definitions.h"
#include "passes/PassesService.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        GlobalDataUBO globalDataUBO{};
        PassesService passesService{context};
        TileInfoUBO tileInfoUBO{};
        std::array<LightData, MAX_LIGHTS> lights{};
        unsigned int lightsCount = 0;
        long long start = -1;
        bool hasToUpdateLights = true;
        bool giSettingsChanged = true;
        unsigned int giFrameCount = 0;

    public:
        void resetFrameCount() {
            giFrameCount = 0;
        }

        void setUpdateLights() {
            hasToUpdateLights = true;
        }

        void setGiSettingsChanged() {
            giSettingsChanged = true;
        }

        void onInitialize() override;

        void updateVoxelData();

        void updateCurrentTime();

        explicit EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        long long currentTimeMs = 0;
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        float deltaTime = 0;

        void updateGlobalData();

        void onSync() override;

        bool shouldClearGIBuffer();

        void updateLights();

        static glm::vec3 CalculateSunColor(float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                           glm::vec3 &middayColor);

        static glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t);
    };
}
#endif
