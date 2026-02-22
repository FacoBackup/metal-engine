#ifndef RAYTRACINGSERVICE_H
#define RAYTRACINGSERVICE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <future>
#include <atomic>
#include <complex.h>
#include <mutex>
#include <entt/entity/registry.hpp>
#include <vulkan/vulkan.h>
#include "../../common/AbstractRuntimeComponent.h"
#include "../../repository/world/components/MeshComponent.h"
#include "../../repository/world/components/TransformComponent.h"

namespace Metal {
    struct BufferInstance;
    struct MeshInstance;

    class RayTracingService final : public AbstractRuntimeComponent {
        struct BLASEntry {
            VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
            std::shared_ptr<BufferInstance> buffer = nullptr;
            std::shared_ptr<BufferInstance> scratchBuffer = nullptr;
        };

        struct MeshBuildData {
            std::string meshId;
            MeshInstance *instance = nullptr;
        };

        struct EntityBuildData {
            std::string meshId;
            glm::mat4 model;
            unsigned int materialIndex;
        };

        // One BLAS per unique mesh ID
        std::unordered_map<std::string, BLASEntry> blasEntries;

        // TLAS
        VkAccelerationStructureKHR tlas = VK_NULL_HANDLE;
        std::shared_ptr<BufferInstance> tlasBuffer = nullptr;
        std::shared_ptr<BufferInstance> instancesBuffer = nullptr;

        // Scratch buffer for TLAS
        std::shared_ptr<BufferInstance> tlasScratchBuffer = nullptr;

        bool accelerationStructureBuilt = false;
        bool needsRebuild = true;

        std::future<void> buildTask;
        std::atomic<bool> isBuilding{false};
        std::mutex blasMutex;

        // Structures for the background thread to fill
        struct BuildResult {
            std::unordered_map<std::string, BLASEntry> newBlasEntries;
            VkAccelerationStructureKHR newTlas = VK_NULL_HANDLE;
            std::shared_ptr<BufferInstance> newTlasBuffer = nullptr;
            std::shared_ptr<BufferInstance> newInstancesBuffer = nullptr;
            std::shared_ptr<BufferInstance> newTlasScratchBuffer = nullptr;
        };

        std::unique_ptr<BuildResult> pendingResult = nullptr;
        std::mutex resultMutex;

        void buildBLAS(std::unordered_map<std::string, BLASEntry> &currentBlas,
                       const std::vector<MeshBuildData> &meshes);

        void buildTLAS(const std::unordered_map<std::string, BLASEntry> &currentBlas,
                       const std::vector<EntityBuildData> &entities, BuildResult &result);

        void destroyAccelerationStructures();

        void asyncBuild(std::vector<MeshBuildData> meshes, std::vector<EntityBuildData> entities);

    public:
        explicit RayTracingService()
            : AbstractRuntimeComponent() {
        }

        ~RayTracingService() override {
            if (buildTask.valid()) {
                buildTask.wait();
            }
            destroyAccelerationStructures();
        }

        void updateBVH();

        void checkForChanges(bool &hasChanged);

        void collectEssentialData(std::vector<MeshBuildData> &meshes,
                                  std::vector<EntityBuildData> &entities,
                                  std::unordered_map<std::string, MeshInstance *> &uniqueMeshes,
                                  MeshComponent &meshComp, TransformComponent &transformComp,
                                  entt::entity entity);

        void onSync() override;

        [[nodiscard]] bool isReady() const {
            return accelerationStructureBuilt;
        }

        void markDirty() {
            needsRebuild = true;
        }

        [[nodiscard]] VkAccelerationStructureKHR getTLAS() const {
            return tlas;
        }
    };
} // Metal

#endif //RAYTRACINGSERVICE_H
