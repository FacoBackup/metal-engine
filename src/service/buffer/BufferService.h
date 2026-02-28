#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include <vector>
#include <memory>
#include "../abstract/AbstractResourceService.h"
#include "BufferInstance.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;
    class VulkanContext;

    class BufferService final : public AbstractResourceService<BufferInstance> {
        void copyBuffer(const BufferInstance *srcBuffer,
                        const BufferInstance *dstBuffer) const;

        void createVkBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                            BufferInstance *buffer) const;

    public:
        [[nodiscard]] unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) const;

        BufferInstance *createBuffer(const std::string &id, VkDeviceSize bufferSize,
                                     VkBufferUsageFlags usageFlags,
                                     VkMemoryPropertyFlags memoryPropertyFlags);

        BufferInstance *createBuffer(const std::string &id, VkDeviceSize dataSize,
                                     VkBufferUsageFlags usageFlags,
                                     const void *bufferData);

        BufferInstance *createBuffer(const std::string &id, VkDeviceSize dataSize,
                                     VkBufferUsageFlags usageFlags,
                                     const void *bufferData,
                                     bool deviceAddress);

        [[nodiscard]] BufferInstance *createBuffer(const std::string &id, VkDeviceSize bufferSize,
                                                   VkBufferUsageFlags usageFlags,
                                                   VkMemoryPropertyFlags memoryPropertyFlags,
                                                   bool deviceAddress);

        void disposeResource(BufferInstance *resource) override;
    };
} // Metal

#endif
