#include "DescriptorService.h"

#include "DescriptorInstance.h"
#include "../../../../common/runtime/ApplicationContext.h"
#include "../../../../common/util/VulkanUtils.h"

namespace Metal {
    DescriptorService::DescriptorService(ApplicationContext &context)
        : AbstractResourceService(context), poolRepository(context.getEngineContext().poolRepository) {
    }

    DescriptorInstance * DescriptorService::createDescritor() {
        auto *instance = new DescriptorInstance;
        registerResource(instance);
        return instance;
    }

    void DescriptorService::onInitialize() {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1; // Adjust this based on your needs

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.flags = 0;
        poolInfo.maxSets = 1; // NUM OF IMAGES
        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(vulkanContext.device.device, &poolInfo,
                                                          nullptr, &poolRepository.descriptorPool));
    }
} // Metal
