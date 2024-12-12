#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include <vector>

#include "../../common/interface/AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;

    class BufferService final : public AbstractResourceService {
        [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;


        void copyBuffer(const BufferInstance *srcBuffer, const BufferInstance *dstBuffer) const;

        void createVkBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                            BufferInstance *buffer) const;

    public:
        explicit BufferService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] BufferInstance *createBuffer(VkDeviceSize bufferSize,
                                                   VkBufferUsageFlags usageFlags,
                                                   VkMemoryPropertyFlags memoryPropertyFlags) const;

        template<class T>
        BufferInstance *createBuffer(std::vector<T> bufferData);
    };
} // Metal

#endif
