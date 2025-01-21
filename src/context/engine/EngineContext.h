#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <chrono>

#include "../../dto/ubo/GlobalDataUBO.h"
#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/ubo/LightData.h"
#include "../../dto/ubo/TileInfoUBO.h"
#include "../../enum/engine-definitions.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        GlobalDataUBO globalDataUBO{};
        TileInfoUBO tileInfoUBO{};
        std::vector<LightData> lights{};
        unsigned int lightsCount = 0;
        long long start = -1;
        bool cameraUpdated = true;
        bool lightingDataUpdated = true;
        bool giSettingsUpdated = true;
        std::string voxelizationRequestId;
        unsigned int giAccumulationCount = 0;

    public:
        void setLightingDataUpdated(const bool val) {
            lightingDataUpdated = val;
        }

        bool isLightingDataUpdated() const {
            return lightingDataUpdated;
        }

        void setCameraUpdated(const bool val) {
            cameraUpdated = val;
        }

        bool isCameraUpdated() const {
            return cameraUpdated;
        }

        void setGISettingsUpdated(const bool val) {
            giSettingsUpdated = val;
        }

        void resetGiAccumulationCount() {
            globalDataUBO.giAccumulationCount = 0;
        }

        bool isGISettingsUpdated() const {
            return giSettingsUpdated;
        }

        void onInitialize() override;

        void updateVoxelData();

        void updateCurrentTime();

        void dispatchSceneVoxelization();

        std::string getVoxelizationRequestId() {
            return voxelizationRequestId;
        }

        explicit EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        long long currentTimeMs = 0;
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        float deltaTime = 0;

        void updateGlobalData();

        void onSync() override;

        void registerBespokeLights(int &index);

        void registerEmissiveLights(int &index);

        void registerSun(int &index);

        void updateLights();

        static glm::vec3 CalculateSunColor(float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                           glm::vec3 &middayColor);

        static glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t);
    };
}
#endif
