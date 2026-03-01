#ifndef DESCRIPTORSETSERVICE_H
#define DESCRIPTORSETSERVICE_H

#include "../abstract/AbstractResourceService.h"
#include "DescriptorInstance.h"

namespace Metal {
    struct PipelineBuilder;
    struct TextureInstance;
    struct FrameBufferInstance;

    class DescriptorSetService final : public AbstractResourceService<DescriptorInstance> {
    public:
        DescriptorInstance *createDescriptor(const PipelineBuilder &pipelineBuilder, const std::string &id, VkShaderStageFlags stageFlags);

        void disposeResource(DescriptorInstance *resource) override;

        std::vector<DescriptorInstance *> getAllDescriptors() const;

        void setImageDescriptor(const FrameBufferInstance *framebuffer, unsigned int attachmentIndex);

        void setImageDescriptor(TextureInstance *texture);

        void updateDescriptor(DescriptorInstance *descriptor);

        void write(DescriptorInstance *descriptor);

        static void Write(const VkDescriptorSet &vkDescriptorSet, const std::vector<DescriptorBinding> &bindings);
    };
}

#endif //DESCRIPTORSETSERVICE_H
