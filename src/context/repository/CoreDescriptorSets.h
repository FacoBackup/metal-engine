#ifndef COREDESCRIPTORSETS_H
#define COREDESCRIPTORSETS_H

#include <vulkan/vulkan_core.h>

#include "AbstractCoreRepository.h"

namespace Metal {
    struct DescriptorInstance;
}

namespace Metal {
    struct CoreDescriptorSets final : AbstractCoreRepository {
        explicit CoreDescriptorSets(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        DescriptorInstance *globalDataDescriptor = nullptr;
        DescriptorInstance *imageSampler = nullptr;

        void onInitialize() override;

        void createDescriptors();

        void updateImageSamplerDescriptor(VkSampler sampler, VkImageView view);

    private:
        VkSampler lastSampler = VK_NULL_HANDLE;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
