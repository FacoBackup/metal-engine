#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "core/CoreBuffers.h"
#include "core/CorePipelines.h"
#include "core/CoreFrameBuffers.h"
#include "repository/backend/CommandPoolRepository.h"
#include "repository/resource/ResourceRepository.h"
#include "service/core/buffer/BufferService.h"
#include "service/core/pipeline/PipelineService.h"
#include "service/core/pools/CommandPoolService.h"
#include "service/core/framebuffer/FrameBufferService.h"
#include "service/core/shader/ShaderService.h"
#include "service/core/texture/TextureService.h"
#include "system/RenderPassSystem.h"
#include "system/camera/CameraSystem.h"

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
    public:
        explicit EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        // ----------- SYSTEMS
        TextureService textureService{context};
        FrameBufferService framebufferService{context};
        PipelineService pipelineService{context};
        ShaderService shaderService{context};
        CameraSystem cameraSystem{context};
        RenderPassSystem renderPassSystem{context};
        CommandPoolService poolService{context};
        BufferService bufferService{context};
        // ----------- SYSTEMS

        // ----------- REPOSITORIES
        CameraRepository cameraRepository{context};
        ResourceRepository resourceRepository{};
        CommandPoolRepository poolRepository{};
        // ----------- REPOSITORIES

        // // ----------- CORE REPOSITORIES
        CoreFrameBuffers coreFrameBuffers{context};
        CorePipelines corePipelines{context};
        CoreBuffers coreBuffers{context};
        // ----------- CORE REPOSITORIES

        void dispose() const;

        void onInitialize() override;

        void onSync() override;

        ResourceRepository &getResourceRepository();
    };
}
#endif
