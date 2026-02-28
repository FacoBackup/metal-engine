#ifndef METAL_ENGINE_BUFFERBUILDER_H
#define METAL_ENGINE_BUFFERBUILDER_H

#include "ResourceBuilder.h"
#include <vulkan/vulkan.h>

namespace Metal {
    class BufferBuilder final : public ResourceBuilder {
        VkDeviceSize size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;

    public:
        explicit BufferBuilder(const std::string &id, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
            : ResourceBuilder(id), size(size), usage(usage), properties(properties) {
        }

        ResourceType getType() override;

        RuntimeResource *build() override;
    };
} // Metal

#endif //METAL_ENGINE_BUFFERBUILDER_H
