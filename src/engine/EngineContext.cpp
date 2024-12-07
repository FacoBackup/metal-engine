#include "EngineContext.h"

namespace Metal {
    void EngineContext::onSync() {
        cameraSystem.onSync();
        renderPassSystem.onSync();
    }

    ResourceRepository &EngineContext::getResourceRepository() {
        return resourceRepository;
    }

    TextureService &EngineContext::getTextureService() {
        return textureService;
    }

    RenderPassService &EngineContext::getRenderPassService() {
        return renderPassService;
    }

    ShaderService &EngineContext::getShaderService() {
        return shaderService;
    }
}
