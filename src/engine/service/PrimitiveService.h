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
    class DirtyStateService;
    class BLASService;
    class VulkanContext;

    class PrimitiveService final : public IService, public IEventMember, public IInit, public ISync {
        MaterialService *materialService = nullptr;
        EngineContext *engineContext = nullptr;
        WorldRepository *worldRepository = nullptr;
        BufferService *bufferService = nullptr;
        DirtyStateService *dirtyStateService = nullptr;
        BLASService *blasService = nullptr;
        VulkanContext *vulkanContext = nullptr;

        std::vector<MeshMetadata> meshMetadata{};
        bool fullRebuildNeeded = false;

        /**
         * @brief Performs a full rebuild of the mesh metadata.
         * This is typically called when the scene structure changes (e.g., BVH rebuild).
         * It clears existing metadata, determines the required buffer size, and re-fetches
         * device addresses for vertex and index data for all active meshes.
         * @return True if any metadata was changed and requires uploading to the GPU.
         */
        bool rebuildMetadata();

        /**
         * @brief Processes entities marked as dirty for material updates.
         * It iterates through entities flagged with DirtyType::Material and updates their
         * corresponding entries in the mesh metadata buffer.
         * @return True if any material data was changed and requires uploading to the GPU.
         */
        bool processDirtyMaterials();

    public:
        PrimitiveService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"MaterialService", &materialService},
                {"EngineContext", &engineContext},
                {"WorldRepository", &worldRepository},
                {"BufferService", &bufferService},
                {"DirtyStateService", &dirtyStateService},
                {"BLASService", &blasService},
                {"VulkanContext", &vulkanContext}
            };
        }

        void onInitialize() override;

        /**
         * @brief Synchronizes the service state with the current engine state.
         * Handles full metadata rebuilds if requested and incremental material updates
         * for dirty entities. Uploads changes to the GPU buffer if necessary.
         */
        void onSync() override;

        /**
         * @brief Forces an update of material data for all active meshes.
         */
        void updateMeshMaterials();

        /**
         * @brief Adds a single metadata entry.
         * @param metadata The metadata to add.
         */
        void addMetadata(const MeshMetadata& metadata);

        /**
         * @brief Clears all currently stored metadata.
         */
        void clearMetadata();

        /**
         * @brief Uploads the current metadata vector to the GPU buffer across all registered frames.
         */
        void uploadMetadata();
    };
}

#endif //PRIMITIVESERVICE_H
