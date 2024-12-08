#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H
#include <vk_mem_alloc.h>

#include "BufferType.h"
#include "../../AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    class BufferInstance;

    class BufferService final : public AbstractResourceService {
    public:
        explicit BufferService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        BufferInstance *createBuffer(BufferType type, VkDeviceSize bufferSize,
                                     VkBufferUsageFlags usage,
                                     VmaMemoryUsage memoryUsage) const;
    };
} // Metal

#endif
