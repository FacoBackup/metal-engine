#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "core/CoreBuffers.h"
#include "core/CorePipelines.h"
#include "core/CoreFrameBuffers.h"
#include "core/GlobalDataUBO.h"
#include "repository/RuntimeRepository.h"
#include "repository/backend/CommandDescriptorRepository.h"
#include "repository/camera/CameraRepository.h"
#include "repository/resource/ResourceRepository.h"
#include "service/camera/CameraMovementService.h"
#include "service/core/buffer/BufferService.h"
#include "service/core/pipeline/PipelineService.h"
#include "service/core/command/CommandService.h"
#include "service/core/descriptor/DescriptorService.h"
#include "service/core/framebuffer/FrameBufferService.h"
#include "service/core/shader/ShaderService.h"
#include "service/core/texture/TextureService.h"
#include "system/RenderPassSystem.h"
#include "system/camera/CameraSystem.h"
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
        TimePoint currentTime;
        TimePoint previousTime = Clock::now();
        GlobalDataUBO globalDataUBO{};

    public:
        explicit EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        float deltaTime = 0;
        bool globalDataNeedsUpdate = false;

        // ----------- SYSTEMS / SERVICE
        TextureService textureService{context};
        CameraMovementService cameraMovementService{context};
        FrameBufferService framebufferService{context};
        PipelineService pipelineService{context};
        ShaderService shaderService{context};
        CameraSystem cameraSystem{context};
        RenderPassSystem renderPassSystem{context};
        CommandService commandService{context};
        BufferService bufferService{context};
        DescriptorService descriptorService{context};
        // ----------- SYSTEMS

        // ----------- REPOSITORIES
        CameraRepository cameraRepository{context};
        ResourceRepository resourceRepository{};
        CommandDescriptorRepository poolRepository{};
        RuntimeRepository runtimeRepository{};
        // ----------- REPOSITORIES

        // // ----------- CORE REPOSITORIES
        CoreFrameBuffers coreFrameBuffers{context};
        CorePipelines corePipelines{context};
        CoreBuffers coreBuffers{context};
        DescriptorInstance *descriptor = nullptr;
        long long start = -1;
        // ----------- CORE REPOSITORIES

        void dispose() const;

        void onInitialize() override;

        void onSync() override;

        ResourceRepository &getResourceRepository();
    };
}
#endif
