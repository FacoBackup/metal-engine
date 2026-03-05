#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "frame-builder/EngineFrame.h"
#include <chrono>
#include <memory>

#include "../../dto/buffers/GlobalDataUBO.h"
#include "../../dto/buffers/AtmosphereUBO.h"
#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/TileInfoUBO.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        GlobalDataUBO globalDataUBO{};
        AtmosphereUBO atmosphereUBO{};
        long long start = -1;
        bool cameraUpdated = true;
        bool updateLights = true;
        bool giSettingsUpdated = true;
        bool isFirstFrame = true;

    public:
        GlobalDataUBO &getGlobalDataUBO() { return globalDataUBO; }

        void setUpdateLights(const bool val) {
            updateLights = val;
        }

        [[nodiscard]] bool isUpdateLights() const {
            return updateLights;
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

        void updateCurrentTime();

        explicit EngineContext() : AbstractRuntimeComponent() {
        }

        long long currentTimeMs = 0;
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        float deltaTime = 0;

        std::vector<EngineFrame *> registeredFrames;
        EngineFrame *currentFrame = nullptr;
        void updateGlobalData();

        void registerFrame(EngineFrame *frame) {
            registeredFrames.push_back(frame);
        }

        void unregisterFrame(EngineFrame *frame) {
            std::erase(registeredFrames, frame);
        }

        void onSync() override;

        void dispose();
    };
}
#endif
