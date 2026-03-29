#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include <functional>
#include <chrono>
#include <memory>
#include <thread>
#include <atomic>

#include <engine/dto/GlobalDataUBO.h>
#include <common/IService.h>
#include <common/ISync.h>
#include <engine/frame-builder/EngineFrame.h>
#include <engine/frame-builder/EngineFrameBuilder.h>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class ApplicationEventContext;
    class TransformService;
    class StreamingService;
    class BLASService;
    class TLASService;
    class MaterialService;
    class CameraService;
    class LightService;
    class PhysicsService;
    class VulkanContext;
    struct WorldRepository;
    struct EditorRepository;
    struct EngineRepository;
    class EngineFrame;
    class EngineFrameBuilder;

    class EngineContext final : public IService, public ISync {
        TransformService *transformService = nullptr;
        StreamingService *streamingService = nullptr;
        BLASService *blasService = nullptr;
        TLASService *tlasService = nullptr;
        MaterialService *materialService = nullptr;
        CameraService *cameraService = nullptr;
        LightService *lightService = nullptr;
        PhysicsService *physicsService = nullptr;
        VulkanContext *vulkanContext = nullptr;
        WorldRepository *worldRepository = nullptr;
        EditorRepository *editorRepository = nullptr;
        EngineRepository *engineRepository = nullptr;

        GlobalDataUBO globalDataUBO{};
        long long start = -1;
        bool cameraUpdated = true;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TransformService", &transformService},
                {"StreamingService", &streamingService},
                {"BLASService", &blasService},
                {"TLASService", &tlasService},
                {"MaterialService", &materialService},
                {"CameraService", &cameraService},
                {"LightService", &lightService},
                {"PhysicsService", &physicsService},
                {"VulkanContext", &vulkanContext},
                {"WorldRepository", &worldRepository},
                {"EditorRepository", &editorRepository},
                {"EngineRepository", &engineRepository}
            };
        }

        GlobalDataUBO &getGlobalDataUBO() { return globalDataUBO; }

        void setCameraUpdated(const bool val) {
            cameraUpdated = val;
        }

        bool isCameraUpdated() const {
            return cameraUpdated;
        }

        void resetPathTracerAccumulationCount() const;

        void updateCurrentTime();

        EngineFrameBuilder createFrame(const std::string &id);

        void setCurrentFrame(const std::string &id);

        long long currentTimeMs = 0;
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        float deltaTime = 0;

        std::vector<EngineFrame *> registeredFrames;
        EngineFrame *currentFrame = nullptr;

        std::function<void()> cameraUpdateCallback;

        void setCameraUpdateCallback(std::function<void()> callback) {
            cameraUpdateCallback = std::move(callback);
        }

        void updateGlobalData();

        void registerFrame(EngineFrame *frame) {
            registeredFrames.push_back(frame);
        }

        void unregisterFrame(EngineFrame *frame) {
            std::erase(registeredFrames, frame);
        }

        void onSync() override;
    };
}
#endif
