#ifndef DESCRIPTORINSTANCE_H
#define DESCRIPTORINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "RuntimeResource.h"

namespace Metal {
    class BufferInstance;

    struct DescriptorInstance final : RuntimeResource {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        bool ready = false;

        void dispose(VulkanContext &context) override;

        void create(const VulkanContext &context, VkDescriptorPool pool);

        void addBufferDescriptor(uint32_t bindingPoint, VkDescriptorType type, BufferInstance *bufferInstance);

        void addImageDescriptor(uint32_t bindingPoint, VkDescriptorType type, VkSampler sampler, VkImageView view);

        void write(const VulkanContext &context) ;

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
    };
} // Metal

#endif //DESCRIPTORINSTANCE_H
