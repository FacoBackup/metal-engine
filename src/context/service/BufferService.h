#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include <vk_mem_alloc.h>

#include "../../common/interface/AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;

    class BufferService final : public AbstractResourceService {
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        void createVkBuffer(BufferInstance *instance, VkMemoryPropertyFlags properties) const;

    public:
        explicit BufferService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] BufferInstance *createBuffer(VkDeviceSize instanceSize,
                                                   uint32_t instanceCount,
                                                   VkBufferUsageFlags usageFlags,
                                                   VkMemoryPropertyFlags memoryPropertyFlags) const;
    };
} // Metal

#endif
