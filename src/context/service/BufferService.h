#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include <vector>
#include "../../common/interface/AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;

    class BufferService final : public AbstractResourceService {
        void copyBuffer(const BufferInstance *srcBuffer, const BufferInstance *dstBuffer) const;

        void createVkBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                            BufferInstance *buffer) const;

    public:
        explicit BufferService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        [[nodiscard]] BufferInstance *createBuffer(VkDeviceSize bufferSize,
                                                   VkBufferUsageFlags usageFlags,
                                                   VkMemoryPropertyFlags memoryPropertyFlags) const;

        BufferInstance *createBuffer(VkDeviceSize dataSize, const void *bufferData) const;
    };
} // Metal

#endif
