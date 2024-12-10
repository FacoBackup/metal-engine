#ifndef DESCRIPTORPOOLSERVICE_H
#define DESCRIPTORPOOLSERVICE_H

#include <vulkan/vulkan_core.h>
#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct DescriptorInstance;

    class DescriptorService final : public AbstractResourceService {
    public:
        explicit DescriptorService(ApplicationContext &context);

        [[nodiscard]] DescriptorInstance *createDescriptor() const;

        void addLayoutBinding(DescriptorInstance *instance, VkShaderStageFlagBits stageFlags,
                              VkDescriptorType descriptorType,
                              unsigned int bindingPoint) const;
    };
} // Metal

#endif //DESCRIPTORPOOLSERVICE_H
