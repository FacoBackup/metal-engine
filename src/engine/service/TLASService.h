#ifndef TLASSERVICE_H
#define TLASSERVICE_H

#include <common/IService.h>
#include <common/IDisposable.h>
#include <common/IInit.h>
#include <common/ISync.h>
#include <common/IEventMember.h>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Metal {
    class VulkanContext;
    class BufferService;
    class BLASService;
    struct WorldRepository;
    class DescriptorSetService;
    class PipelineService;
    struct BufferInstance;

    class TLASService final : public IService, public IEventMember, public IInit, public ISync {
        VulkanContext *vulkanContext = nullptr;
        BufferService *bufferService = nullptr;
        BLASService *blasService = nullptr;
        WorldRepository *worldRepository = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
        PipelineService *pipelineService = nullptr;

        VkAccelerationStructureKHR tlas = VK_NULL_HANDLE;
        BufferInstance *tlasBuffer = nullptr;
        BufferInstance *instancesBuffer = nullptr;
        BufferInstance *tlasScratchBuffer = nullptr;

        bool needsRebuild = true;

        VkDeviceAddress getDeviceAddress(VkBuffer buffer) const;

        void updateDescriptorSets(VkAccelerationStructureKHR asHandle) const;

    public:
        TLASService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"BufferService", &bufferService},
                {"BLASService", &blasService},
                {"WorldRepository", &worldRepository},
                {"DescriptorSetService", &descriptorSetService},
                {"PipelineService", &pipelineService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void dispose();

        void buildTLAS();

        [[nodiscard]] VkAccelerationStructureKHR getTLAS() const { return tlas; }
    };
}

#endif //TLASSERVICE_H
