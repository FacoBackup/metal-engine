#ifndef DESCRIPTORSETSERVICE_H
#define DESCRIPTORSETSERVICE_H

#include "../../common/AbstractResourceService.h"
#include "../dto/DescriptorInstance.h"

namespace Metal {
    struct PipelineBuilder;
    struct TextureInstance;
    struct FrameBufferInstance;
    class VulkanContext;
    class FrameBufferService;
    class BufferService;
    class TextureService;

    class DescriptorSetService final : public AbstractResourceService<DescriptorInstance> {
        VulkanContext &vulkanContext;
        FrameBufferService &framebufferService;
        BufferService &bufferService;
        TextureService &textureService;

    public:
        DescriptorSetService(VulkanContext &vulkanContext, FrameBufferService &framebufferService, BufferService &bufferService, TextureService &textureService)
            : vulkanContext(vulkanContext), framebufferService(framebufferService), bufferService(bufferService), textureService(textureService) {}
        DescriptorSetService() = delete;

        DescriptorInstance *createDescriptor(const PipelineBuilder &pipelineBuilder, const std::string &id, VkShaderStageFlags stageFlags);

        void disposeResource(DescriptorInstance *resource) override;

        std::vector<DescriptorInstance *> getAllDescriptors() const;

        void setImageDescriptor(const FrameBufferInstance *framebuffer, unsigned int attachmentIndex);

        void setImageDescriptor(TextureInstance *texture);

        void updateDescriptor(DescriptorInstance *descriptor);

        void write(DescriptorInstance *descriptor);

        static void Write(VulkanContext &vulkanContext, BufferService &bufferService, const VkDescriptorSet &vkDescriptorSet, const std::vector<DescriptorBinding> &bindings);
    };
}

#endif //DESCRIPTORSETSERVICE_H
