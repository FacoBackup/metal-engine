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

    FrameBufferService &EngineContext::getFrameBufferService() {
        return frameBufferService;
    }

    ShaderService &EngineContext::getShaderService() {
        return shaderService;
    }
}
