#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "core/CorePipelines.h"
#include "core/CoreRenderPasses.h"
#include "repository/resource/ResourceRepository.h"
#include "service/pipeline/PipelineService.h"
#include "service/render-pass/RenderPassService.h"
#include "service/shader/ShaderService.h"
#include "service/texture/TextureService.h"
#include "system/RenderPassSystem.h"
#include "system/camera/CameraSystem.h"

namespace Metal {
    class EngineContext final : public AbstractRuntimeComponent {
    public:
        explicit EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        // ----------- SYSTEMS
        TextureService textureService{context};
        RenderPassService renderPassService{context};
        PipelineService pipelineService{context};
        ShaderService shaderService{context};
        CameraSystem cameraSystem{context};
        RenderPassSystem renderPassSystem{context};
        // ----------- SYSTEMS

        // ----------- REPOSITORIES
        CameraRepository cameraRepository{context};
        ResourceRepository resourceRepository{};
        // ----------- REPOSITORIES

        // // ----------- CORE REPOSITORIES
        CoreRenderPasses coreRenderPasses{context};
        CorePipelines corePipelines{context};
        // ----------- CORE REPOSITORIES

        void dispose() const;

        void onInitialize() override;

        void onSync() override;

        ResourceRepository &getResourceRepository();

        TextureService &getTextureService();

        RenderPassService &getRenderPassService();

        ShaderService &getShaderService();

        PipelineService &getPipelineService();
    };
}
#endif
