#include "EngineContext.h"

#include "../common/runtime/ApplicationContext.h"

namespace Metal {
    void EngineContext::dispose() const {
        for (auto &resource: resourceRepository.resources) {
            resource.second->dispose(context.getVulkanContext());
            delete resource.second;
        }
        vkDestroyDescriptorPool(context.getVulkanContext().device.device, poolRepository.descriptorPool,
                                nullptr);

        vkDestroyCommandPool(context.getVulkanContext().device.device, poolRepository.commandPool,
                             nullptr);
    }

    void EngineContext::onInitialize() {
        textureService.onInitialize();
        framebufferService.onInitialize();
        pipelineService.onInitialize();
        shaderService.onInitialize();
        cameraSystem.onInitialize();
        renderPassSystem.onInitialize();
        poolService.onInitialize();

        coreFrameBuffers.onInitialize();
        corePipelines.onInitialize();
    }

    void EngineContext::onSync() {
        cameraSystem.onSync();
        renderPassSystem.onSync();
    }

    ResourceRepository &EngineContext::getResourceRepository() {
        return resourceRepository;
    }
}
