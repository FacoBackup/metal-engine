#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include "../../common/interface/AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;

    class BufferService final : public AbstractResourceService {
        [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        void createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                            BufferInstance *buffer) const;

    public:
        explicit BufferService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] BufferInstance *createBuffer(VkDeviceSize bufferSize,
                                                   VkBufferUsageFlags usageFlags,
                                                   VkMemoryPropertyFlags memoryPropertyFlags) const;
    };
} // Metal

#endif
