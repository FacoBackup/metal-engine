#include "EngineContext.h"

#include "../common/runtime/ApplicationContext.h"

namespace Metal {
    void EngineContext::dispose() const {
        for (auto &resource: resourceRepository.resources) {
            resource.second->dispose(context.getVulkanContext());
            delete resource.second;
        }
    }

    void EngineContext::onInitialize() {
        coreRenderPasses.onInitialize();
        corePipelines.onInitialize();
    }

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

    PipelineService &EngineContext::getPipelineService() {
        return pipelineService;
    }
}
