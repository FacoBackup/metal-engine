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
        bool updateLights = true;
        bool updateVolumes = true;
        bool giSettingsUpdated = true;

    public:
        GlobalDataUBO &getGlobalDataUBO() { return globalDataUBO; }

        void setUpdateLights(const bool val) {
            updateLights = val;
        }

        [[nodiscard]] bool isUpdateLights() const {
            return updateLights;
        }

        void setUpdateVolumes(const bool val) {
            updateVolumes = val;
        }

        [[nodiscard]] bool isUpdateVolumes() const {
            return updateVolumes;
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

        void resetPathTracerAccumulationCount() const;

        bool isGISettingsUpdated() const {
            return giSettingsUpdated;
        }

        void onInitialize() override;

        void updateTileData();

        void updateCurrentTime();

        explicit EngineContext() : AbstractRuntimeComponent() {
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
