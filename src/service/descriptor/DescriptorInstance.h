#ifndef DESCRIPTORINSTANCE_H
#define DESCRIPTORINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/abstract/RuntimeResource.h"

namespace Metal {
    struct BufferInstance;

    struct DescriptorInstance final {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        bool ready = false;

        void dispose(const VulkanContext &context) const;

        void create(const VulkanContext &context);

        void addLayoutBinding(VkShaderStageFlagBits stageFlags,
                              VkDescriptorType descriptorType,
                              uint32_t bindingPoint);

        void addBufferDescriptor(uint32_t bindingPoint, VkDescriptorType type,
                                 const std::shared_ptr<BufferInstance> &bufferInstance);

        void addImageDescriptor(uint32_t bindingPoint, VkDescriptorType type, VkSampler sampler, VkImageView view);

        void write(const VulkanContext &context);

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
    };
} // Metal

#endif //DESCRIPTORINSTANCE_H
