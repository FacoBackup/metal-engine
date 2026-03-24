#ifndef PRIMITIVESERVICE_H
#define PRIMITIVESERVICE_H

#include <common/IService.h>
#include <common/IInit.h>
#include <common/ISync.h>
#include <common/IEventMember.h>
#include <vulkan/vulkan.h>
#include <engine/dto/MeshMetadata.h>

namespace Metal {
    class MaterialService;
    class EngineContext;
    struct WorldRepository;
    class BufferService;

    class PrimitiveService final : public IService, public IEventMember, public IInit, public ISync {
        MaterialService *materialService = nullptr;
        EngineContext *engineContext = nullptr;
        WorldRepository *worldRepository = nullptr;
        BufferService *bufferService = nullptr;

        std::vector<MeshMetadata> meshMetadata{};
        bool needsUpdate = true;

        VkDeviceAddress getDeviceAddress(VkBuffer buffer) const;

    public:
        PrimitiveService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"MaterialService", &materialService},
                {"EngineContext", &engineContext},
                {"WorldRepository", &worldRepository},
                {"BufferService", &bufferService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void updateMeshMaterials();

        void addMetadata(const MeshMetadata& metadata);
        void clearMetadata();
        void uploadMetadata();
    };
}

#endif //PRIMITIVESERVICE_H
