#include "BufferInstance.h"

#include <cassert>
#include <cstring>

// THANKS TO https://github.com/blurrypiano/littleVulkanEngine/blob/master/littleVulkanEngine/tutorial19
namespace Metal {
    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the vkBuffer mapping call
     */
    VkDeviceSize BufferInstance::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    BufferInstance::BufferInstance(
            VkDevice device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment)
            : vkDevice(device),
              vkInstanceSize{instanceSize},
              instanceCount{instanceCount},
              vkUsageFlags{usageFlags},
              vkMemoryPropertyFlags{memoryPropertyFlags} {
        vkAlignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        vkBufferSize = vkAlignmentSize * instanceCount;
    }

    /**
     * Map a vkDeviceMemory range of this vkBuffer. If successful, mapped points to the specified vkBuffer range.
     *
     * @param size (Optional) Size of the vkDeviceMemory range to map. Pass VK_WHOLE_SIZE to map the complete
     * vkBuffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the vkBuffer mapping call
     */
    VkResult BufferInstance::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(vkBuffer && vkDeviceMemory && "Called map on vkBuffer before create");
        return vkMapMemory(vkDevice, vkDeviceMemory, offset, size, 0, &mapped);
    }

    /**
     * Unmap a mapped vkDeviceMemory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void BufferInstance::unmap() {
        if (mapped) {
            vkUnmapMemory(vkDevice, vkDeviceMemory);
            mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the mapped vkBuffer. Default value writes whole vkBuffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete vkBuffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void BufferInstance::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        assert(mapped && "Cannot copy to unmapped vkBuffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(mapped, data, vkBufferSize);
        } else {
            char *memOffset = (char *) mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a vkDeviceMemory range of the vkBuffer to make it visible to the device
     *
     * @note Only required for non-coherent vkDeviceMemory
     *
     * @param size (Optional) Size of the vkDeviceMemory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete vkBuffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult BufferInstance::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = vkDeviceMemory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(vkDevice, 1, &mappedRange);
    }

    /**
     * Invalidate a vkDeviceMemory range of the vkBuffer to make it visible to the host
     *
     * @note Only required for non-coherent vkDeviceMemory
     *
     * @param size (Optional) Size of the vkDeviceMemory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete vkBuffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult BufferInstance::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = vkDeviceMemory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(vkDevice, 1, &mappedRange);
    }

    /**
     * Create a vkBuffer info descriptor
     *
     * @param size (Optional) Size of the vkDeviceMemory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo BufferInstance::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{
                vkBuffer,
                offset,
                size,
        };
    }

    /**
     * Copies "vkInstanceSize" bytes of data to the mapped vkBuffer at an offset of index * vkAlignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void BufferInstance::writeToIndex(void *data, int index) {
        writeToBuffer(data, vkInstanceSize, index * vkAlignmentSize);
    }

    /**
     *  Flush the vkDeviceMemory range at index * vkAlignmentSize of the vkBuffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult BufferInstance::flushIndex(int index) { return flush(vkAlignmentSize, index * vkAlignmentSize); }

    /**
     * Create a vkBuffer info descriptor
     *
     * @param index Specifies the region given by index * vkAlignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo BufferInstance::descriptorInfoForIndex(int index) {
        return descriptorInfo(vkAlignmentSize, index * vkAlignmentSize);
    }

    /**
     * Invalidate a vkDeviceMemory range of the vkBuffer to make it visible to the host
     *
     * @note Only required for non-coherent vkDeviceMemory
     *
     * @param index Specifies the region to invalidate: index * vkAlignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult BufferInstance::invalidateIndex(int index) {
        return invalidate(vkAlignmentSize, index * vkAlignmentSize);
    }

    void BufferInstance::dispose(VulkanContext &context) {
        unmap();
        vkDestroyBuffer(vkDevice, vkBuffer, nullptr);
        vkFreeMemory(vkDevice, vkDeviceMemory, nullptr);
    }
} // Metal
