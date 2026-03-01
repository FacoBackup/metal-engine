#ifndef METAL_ENGINE_BUFFERBUILDER_H
#define METAL_ENGINE_BUFFERBUILDER_H

#include "ResourceBuilder.h"
#include <vulkan/vulkan.h>

#include "../../../../service/buffer/BufferInstance.h"

namespace Metal {
    class BufferBuilder final : public ResourceBuilder {
        VkDeviceSize size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;
        BufferType type;

    public:
        explicit BufferBuilder(const std::string &id, VkDeviceSize size, VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties, BufferType type)
            : ResourceBuilder(id), size(size), usage(usage), properties(properties), type(type) {
        }

        ResourceType getType() override;

        RuntimeResource *build() override;
    };
} // Metal

#endif //METAL_ENGINE_BUFFERBUILDER_H
