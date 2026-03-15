#ifndef METAL_ENGINE_BUFFERBUILDER_H
#define METAL_ENGINE_BUFFERBUILDER_H

#include "ResourceBuilder.h"
#include <vulkan/vulkan.h>

#include "../../resource/BufferInstance.h"

namespace Metal {
    class BufferService;

    class BufferBuilder final : public ResourceBuilder {
        VkDeviceSize size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;
        BufferType type;
        BufferService *bufferService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"BufferService", &bufferService}};
        }

        explicit BufferBuilder(const std::string &id, VkDeviceSize size, VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties, BufferType type)
            : ResourceBuilder(id), size(size), usage(usage), properties(properties), type(type) {
        }

        ResourceType getType() override;

        RuntimeResource *build() override;
    };
} // Metal

#endif //METAL_ENGINE_BUFFERBUILDER_H
