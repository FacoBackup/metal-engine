#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "repository/resource/ResourceRepository.h"
#include "service/framebuffer/FrameBufferService.h"
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
        FrameBufferService frameBufferService{context};
        ShaderService shaderService{context};
        CameraSystem cameraSystem{context};
        RenderPassSystem renderPassSystem{context};
        // ----------- SYSTEMS

        // ----------- REPOSITORIES
        CameraRepository cameraRepository{context};
        ResourceRepository resourceRepository{};
        // ----------- REPOSITORIES

        void onInitialize() override;

        void onSync() override;

        ResourceRepository &getResourceRepository();

        TextureService &getTextureService();

        FrameBufferService &getFrameBufferService();

        ShaderService &getShaderService();
    };
}
#endif
