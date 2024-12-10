#include "DescriptorService.h"

#include "../runtime/DescriptorInstance.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"

namespace Metal {
    DescriptorService::DescriptorService(ApplicationContext &context)
        : AbstractResourceService(context) {
    }

    void DescriptorService::addLayoutBinding(DescriptorInstance *instance, VkShaderStageFlagBits stageFlags,
                                             VkDescriptorType descriptorType, uint32_t bindingPoint) {
        auto &samplerLayoutBinding = instance->descriptorSetLayoutBindings.emplace_back();
        samplerLayoutBinding.binding = bindingPoint; // Binding index in the shader
        samplerLayoutBinding.descriptorType = descriptorType;
        samplerLayoutBinding.descriptorCount = 1; // Single texture
        samplerLayoutBinding.stageFlags = stageFlags; // Used in fragment shader
        samplerLayoutBinding.pImmutableSamplers = nullptr; // Set this if you're using immutable samplers
        context.getVulkanContext().registerDescriptorBinding(descriptorType);
    }

    DescriptorInstance *DescriptorService::createDescriptor() const {
        auto *instance = new DescriptorInstance;
        registerResource(instance);
        context.getVulkanContext().registerDescriptorSet();
        return instance;
    }
} // Metal
