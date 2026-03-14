#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <chrono>
#include <memory>

#include "dto/GlobalDataUBO.h"
#include "../common/IService.h"
#include "../common/IDisposable.h"
#include "../common/ISync.h"
#include "frame-builder/EngineFrame.h"
#include "frame-builder/EngineFrameBuilder.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class TransformService;
    class StreamingService;
    class RayTracingService;
    class CameraService;
    class LightService;
    class VolumeService;
    struct WorldRepository;
    struct EditorRepository;
    struct EngineRepository;
    class EngineFrame;
    class EngineFrameBuilder;

    class EngineContext final : public IService, public ISync, public IDisposable {
        TransformService *transformService = nullptr;
        StreamingService *streamingService = nullptr;
        RayTracingService *rayTracingService = nullptr;
        CameraService *cameraService = nullptr;
        LightService *lightService = nullptr;
        VolumeService *volumeService = nullptr;
        WorldRepository *worldRepository = nullptr;
        EditorRepository *editorRepository = nullptr;
        EngineRepository *engineRepository = nullptr;

        GlobalDataUBO globalDataUBO{};
        long long start = -1;
        bool cameraUpdated = true;
        bool giSettingsUpdated = true;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TransformService", transformService},
                {"StreamingService", streamingService},
                {"RayTracingService", rayTracingService},
                {"CameraService", cameraService},
                {"LightService", lightService},
                {"VolumeService", volumeService},
                {"WorldRepository", worldRepository},
                {"EditorRepository", editorRepository},
                {"EngineRepository", engineRepository}
            };
        }

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

        void updateCurrentTime();

        EngineFrameBuilder createFrame(const std::string &id);

        void setCurrentFrame(const std::string & id);

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

        void dispose() override;
    };
}
#endif
