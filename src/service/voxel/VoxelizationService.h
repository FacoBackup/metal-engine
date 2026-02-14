#ifndef VOXELIZATIONSERVICE_H
#define VOXELIZATIONSERVICE_H

#include <string>
#include <thread>
#include <unordered_map>

#include "../../common/AbstractRuntimeComponent.h"
#include "impl/OctreeNode.h"
#include "impl/SparseVoxelOctreeData.h"

#include <atomic>
#include <mutex>
#include <memory>

namespace Metal {
    struct WorldTile;
    struct VoxelizationRequest;
    struct TextureData;
    struct MeshComponent;
    class SparseVoxelOctreeBuilder;
    struct MeshData;

    class VoxelizationService final : public AbstractRuntimeComponent {
        static void FillStorage(SparseVoxelOctreeBuilder &builder, unsigned int &bufferIndex,
                                unsigned int &materialBufferIndex,
                                SparseVoxelOctreeData &voxels, OctreeNode *node);

        static void PutData(unsigned int &bufferIndex, OctreeNode *node);

        void serialize(SparseVoxelOctreeBuilder &builder) const;

        void beginVoxelization();

        void voxelizeGroup(const std::vector<VoxelizationRequest> &request) const;

        void collectRequests(WorldTile &t, std::vector<std::vector<VoxelizationRequest>> &requests) const;

    public:
        mutable std::mutex buildersMutex;
        mutable std::mutex texturesMutex;
        std::unordered_map<std::string, TextureData *> textures{};
        std::unordered_map<std::string, std::unique_ptr<SparseVoxelOctreeBuilder>> builders{};
        std::string localVoxelizationRequestId;
        std::atomic<bool> isVoxelizationDone{false};
        std::atomic<bool> isVoxelizationCancelled{false};
        std::atomic<bool> isExecutingThread{false};
        std::array<std::thread, 3> threads{};
        std::thread mainThread;

        explicit VoxelizationService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        bool isExecutingVoxelization() const {
            return isExecutingThread;
        }

        void onSync() override;

        void cancelRequest();
    };
} // Metal

#endif //VOXELIZERSERVICE_H
