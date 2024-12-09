#ifndef DESCRIPTORINSTANCE_H
#define DESCRIPTORINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../../repository/resource/RuntimeResource.h"

namespace Metal {
    struct DescriptorInstance final : RuntimeResource {
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        bool ready = false;

        void dispose(VulkanContext &context) override;

        void create(const VulkanContext &context, VkDescriptorPool pool);

        void addLayoutBinding(VkShaderStageFlagBits stageFlags, VkDescriptorType descriptorType, uint32_t bindingPoint);

        void addBufferDescriptor(uint32_t bindingPoint, VkDescriptorType type, VkBuffer buffer, uint64_t offset);

        void addImageDescriptor(uint32_t bindingPoint, VkDescriptorType type, VkSampler sampler, VkImageView view);

        void write(const VulkanContext &context) ;

    private:
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
    };
} // Metal

#endif //DESCRIPTORINSTANCE_H
