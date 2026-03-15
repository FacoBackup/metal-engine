#ifndef FRAMEBUFFERATTACHMENT_H
#define FRAMEBUFFERATTACHMENT_H
#include "../dto/DescriptorInstance.h"

namespace Metal {
    class DescriptorSetService;

    struct FrameBufferAttachment final : IContextMember {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_MAX_ENUM;
        DescriptorInstance *imageDescriptor = nullptr;
        bool depth = false;

        void dispose() const;

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"DescriptorSetService", &descriptorSetService}
            };
        }

    private:
        VulkanContext *vulkanContext = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
    };
}

#endif
