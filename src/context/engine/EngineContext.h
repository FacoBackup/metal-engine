#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "frame-builder/EngineFrame.h"
#include <chrono>
#include <memory>

#include "../../dto/buffers/GlobalDataUBO.h"
#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/TileInfoUBO.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        GlobalDataUBO globalDataUBO{};
        long long start = -1;
        bool cameraUpdated = true;
        bool giSettingsUpdated = true;

    public:
        GlobalDataUBO &getGlobalDataUBO() { return globalDataUBO; }

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
