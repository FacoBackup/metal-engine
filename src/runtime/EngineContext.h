#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <chrono>

#include "../renderer/data/GlobalDataUBO.h"
#include "../common/AbstractRuntimeComponent.h"
#include "../renderer/data/BufferInstance.h"
#include "../renderer/data/TLASBuffer.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        std::vector<TLASBuffer> rtTopLevelStructures;
        GlobalDataUBO globalDataUBO{};
        long long start = -1;
        bool cameraUpdated = true;
        bool worldChange = true;
        bool lightVolumeDataNeedsUpdate = true;
        bool giSettingsUpdated = true;
        unsigned int accumulationCount = 0;
        unsigned int rtTLASCount = 0;
        bool isBVHReady = false;
        bool isTLASReady = false;

    public:
        GlobalDataUBO &getGlobalDataUBO() { return globalDataUBO; }

        void setLightVolumeDataNeedsUpdate(const bool val) {
            lightVolumeDataNeedsUpdate = val;
        }

        [[nodiscard]] bool isLightingDataUpdated() const {
            return lightVolumeDataNeedsUpdate;
        }

        void setCameraUpdated(const bool val) {
            cameraUpdated = val;
        }

        void setWorldChange(const bool worldChange) {
            this->worldChange = worldChange;
        }

        bool isCameraUpdated() const {
            return cameraUpdated;
        }

        [[nodiscard]]  int getAccumulationCount() const {
            return accumulationCount;
        }

        bool hasWorldChanged() const {
            return worldChange;
        }

        void setGISettingsUpdated(const bool val) {
            giSettingsUpdated = val;
        }

        void resetAccumulationCount() {
            accumulationCount = 0;
        }

        bool isGISettingsUpdated() const {
            return giSettingsUpdated;
        }

        void onInitialize() override;

        void updateCurrentTime();

        void dispatchBVHBuild();

        void dispatchTLASUpdate();

        void updateTLASInternal();

        long long currentTimeMs = 0;
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        float deltaTime = 0;

        void updateGlobalData();

        void onSync() override;
    };
}
#endif
