#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include <vector>
#include <memory>
#include "../../common/AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;
    class VulkanContext;

    class BufferService final : public AbstractRuntimeComponent {
        void copyBuffer(const std::shared_ptr<BufferInstance> &srcBuffer,
                        const std::shared_ptr<BufferInstance> &dstBuffer) const;

        void createVkBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                            const std::shared_ptr<BufferInstance> &buffer) const;

    public:
        [[nodiscard]] unsigned int  findMemoryType(unsigned int  typeFilter, VkMemoryPropertyFlags properties) const;

        [[nodiscard]] std::shared_ptr<BufferInstance> createBuffer(VkDeviceSize bufferSize,
                                                                   VkBufferUsageFlags usageFlags,
                                                                   VkMemoryPropertyFlags memoryPropertyFlags) const;

        std::shared_ptr<BufferInstance> createBuffer(VkDeviceSize dataSize,
                                                     VkBufferUsageFlags usageFlags,
                                                     const void *bufferData) const;
    };
} // Metal

#endif
