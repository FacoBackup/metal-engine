#ifndef BLASSERVICE_H
#define BLASSERVICE_H

#include <common/AbstractResourceService.h>
#include <common/IInit.h>
#include <common/ISync.h>
#include <engine/resource/BLASInstance.h>
#include <vulkan/vulkan.h>

namespace Metal {
    class VulkanContext;
    class BufferService;
    class MeshService;

    class BLASService final : public AbstractResourceService<BLASInstance>, public IInit, public ISync {
        VulkanContext *vulkanContext = nullptr;
        BufferService *bufferService = nullptr;
        MeshService *meshService = nullptr;

        bool needsRebuild = true;

        VkDeviceAddress getDeviceAddress(VkBuffer buffer) const;

    public:
        BLASService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"BufferService", &bufferService},
                {"MeshService", &meshService}
            };
        }

        void onInitialize() override;

        void onSync();

        void disposeResource(BLASInstance *resource) override;

        BLASInstance *buildBLAS(const std::string &meshId);

        VkDeviceAddress getBLASAddress(const std::string &meshId);
    };
}

#endif //BLASSERVICE_H
