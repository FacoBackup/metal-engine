#include "EngineContext.h"

#include "../common/runtime/ApplicationContext.h"
#include "../common/util/VulkanUtils.h"
#include "service/core/buffer/BufferInstance.h"
#include "service/core/descriptor/DescriptorInstance.h"
#include "service/core/framebuffer/FrameBufferAttachment.h"
#include "service/core/framebuffer/FrameBufferInstance.h"

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
        descriptorService.onInitialize();
        commandService.onInitialize();
        coreFrameBuffers.onInitialize();
        corePipelines.onInitialize();

        descriptor = descriptorService.createDescritor();

        descriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        descriptor->create(context.getVulkanContext(), poolRepository.descriptorPool);

        descriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                       coreFrameBuffers.auxRenderPass->attachments[0]->vkImageSampler,
                                       coreFrameBuffers.auxRenderPass->attachments[0]->vkImageView);
        descriptor->write(context.getVulkanContext());
    }

    void EngineContext::onSync() {
        currentTime = Clock::now();
        deltaTime = static_cast<float>((currentTime - previousTime).count()) / 1000.f;

        if (start == -1) {
            start = Clock::now().time_since_epoch().count();
        }

        cameraSystem.onSync();

        if (globalDataNeedsUpdate) {
            globalDataNeedsUpdate = false;
            globalDataUBO.proj = cameraRepository.projectionMatrix;
            globalDataUBO.view = cameraRepository.viewMatrix;
            globalDataUBO.viewProj = cameraRepository.viewProjectionMatrix;
            globalDataUBO.invProj = cameraRepository.invProjectionMatrix;
            globalDataUBO.invView = cameraRepository.invViewMatrix;

            coreBuffers.globalData->updateData(context.getVulkanContext(), &globalDataUBO);
        }
        renderPassSystem.onSync();
    }

    ResourceRepository &EngineContext::getResourceRepository() {
        return resourceRepository;
    }
}
