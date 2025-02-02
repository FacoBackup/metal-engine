#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <chrono>

#include "../../dto/buffers/GlobalDataUBO.h"
#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/TileInfoUBO.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        GlobalDataUBO globalDataUBO{};
        TileInfoUBO tileInfoUBO{};
        long long start = -1;
        bool cameraUpdated = true;
        bool lightingDataUpdated = true;
        bool volumeDataUpdated = true;
        bool giSettingsUpdated = true;
        std::string voxelizationRequestId;
        unsigned int giAccumulationCount = 0;

    public:
        GlobalDataUBO &getGlobalDataUBO() { return globalDataUBO; }

        void setLightingDataUpdated(const bool val) {
            lightingDataUpdated = val;
        }

        void setVolumeDataUpdated(const bool val) {
            volumeDataUpdated  = val;
        }

        [[nodiscard]] bool isLightingDataUpdated() const {
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
    };
}
#endif
