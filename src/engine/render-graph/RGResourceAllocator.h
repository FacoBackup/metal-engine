#ifndef RGRESOURCEALLOCATOR_H
#define RGRESOURCEALLOCATOR_H

#include <vector>
#include <map>
#include <vulkan/vulkan_core.h>
#include "RGResourceHandle.h"
#include "../../common/IContextMember.h"
#include "../../core/VulkanContext.h"
#include <stdexcept>

namespace Metal {
    struct LivenessInterval {
        uint32_t start = 0xFFFFFFFF;
        uint32_t end = 0;
    };

    class RGResourceAllocator : public IContextMember {
        VulkanContext* vulkanContext = nullptr;
        
        struct MemoryBlock {
            VkDeviceMemory memory;
            VkDeviceSize size;
            uint32_t memoryTypeIndex;
        };
        
        std::vector<MemoryBlock> allocatedBlocks;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"VulkanContext", &vulkanContext}};
        }

        struct AllocationResult {
            VkDeviceMemory memory;
            VkDeviceSize offset;
        };

        AllocationResult allocateImageMemory(VkImage image, VkMemoryPropertyFlags properties) {
            VkMemoryRequirements memReqs;
            vkGetImageMemoryRequirements(vulkanContext->device.device, image, &memReqs);
            return allocateInternal(memReqs, properties);
        }

        AllocationResult allocateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags properties) {
            VkMemoryRequirements memReqs;
            vkGetBufferMemoryRequirements(vulkanContext->device.device, buffer, &memReqs);
            return allocateInternal(memReqs, properties);
        }

        void cleanUp() {
            if (!vulkanContext) return;
            for (auto& block : allocatedBlocks) {
                vkFreeMemory(vulkanContext->device.device, block.memory, nullptr);
            }
            allocatedBlocks.clear();
        }

    private:
        AllocationResult allocateInternal(VkMemoryRequirements memReqs, VkMemoryPropertyFlags properties) {
            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memReqs.size;
            allocInfo.memoryTypeIndex = findMemoryType(memReqs.memoryTypeBits, properties);

            VkDeviceMemory memory;
            if (vkAllocateMemory(vulkanContext->device.device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
                throw std::runtime_error("Failed to allocate memory for RGResource!");
            }
            allocatedBlocks.push_back({memory, memReqs.size, allocInfo.memoryTypeIndex});
            
            return {memory, 0};
        }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
            for (uint32_t i = 0; i < vulkanContext->physicalDeviceMemoryProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) && (vulkanContext->physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }
            throw std::runtime_error("failed to find suitable memory type for RGResource!");
        }
    };
}

#endif // RGRESOURCEALLOCATOR_H
