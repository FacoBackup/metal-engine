#ifndef DESCRIPTORSETSERVICE_H
#define DESCRIPTORSETSERVICE_H

#include "../../common/AbstractResourceService.h"
#include "../dto/DescriptorInstance.h"

namespace Metal {
    struct PipelineBuilder;
    struct TextureInstance;
    struct RenderTargetInstance;
    class VulkanContext;
    class RenderTargetService;
    class BufferService;
    class TextureService;

    class DescriptorSetService final : public AbstractResourceService<DescriptorInstance> {
        VulkanContext *vulkanContext = nullptr;
        RenderTargetService *RenderTargetService = nullptr;
        BufferService *bufferService = nullptr;
        TextureService *textureService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"RenderTargetService", &RenderTargetService},
                {"BufferService", &bufferService},
                {"TextureService", &textureService}
            };
        }

        DescriptorInstance *createDescriptor(const PipelineBuilder &pipelineBuilder, const std::string &id,
                                             VkShaderStageFlags stageFlags);

        void disposeResource(DescriptorInstance *resource) override;

        std::vector<DescriptorInstance *> getAllDescriptors() const;

        void setImageDescriptor(const RenderTargetInstance *framebuffer, unsigned int attachmentIndex);

        void setImageDescriptor(TextureInstance *texture);

        void updateDescriptor(DescriptorInstance *descriptor);

        void write(DescriptorInstance *descriptor);

        void Write(const VkDescriptorSet &vkDescriptorSet, const std::vector<DescriptorBinding> &bindings);
    };
}

#endif //DESCRIPTORSETSERVICE_H
